#include "ViewportWindow.hpp"
#include "Entity.hpp"

#include <iostream>

namespace cw {

void ViewportWindow::OnSetup() {
    m_SceneContext->SetViewportContext(this);

    m_Camera.SetContext(this);
    m_Renderer.SetContext(this);
    m_CollisionResolver.SetContext(this);

    m_CreationTool.SetContext({m_SceneContext, this});
    m_SelectionTool.SetContext({m_SceneContext, this});

    m_Renderer.OnSetup();
    m_Camera.OnSetup();

    m_SelectionIcon = LoadTexture("../assets/icons/selection.png");
    m_LineIcon      = LoadTexture("../assets/icons/line.png"     );
    m_RectangleIcon = LoadTexture("../assets/icons/rectangle.png");
    m_EllipseIcon   = LoadTexture("../assets/icons/ellipse.png"  );

    // m_Image = LoadTexture("C:/Users/yuri/Documents/University/Computer_Graphics/course_work/resources/parrots.png");
}

void ViewportWindow::OnUpdate() {
    m_Camera.OnUpdate();
    m_Renderer.OnUpdate();

    if (m_Data.IsHovered || m_Data.IsFocused) {
        if (m_Data.MouseInBounds) {
            if (m_SelectionTool.IsActive())
                m_SelectionTool.OnUpdate();
            if (m_CreationTool.IsActive())
                m_CreationTool.OnUpdate();
        }
        HandleInput();
    }

    HandleViewportResize();
}

void ViewportWindow::OnRender() {
    
    auto camera = (Camera2D)m_Camera;
    // ClearBackground(Color{40, 40, 40, 255});

    // const float grid_step = 100.0f * camera->zoom;
    // // for (float x = fmodf(camera->offset.x))
    // for (float x = m_Context->GetPosition().x; x < m_Context->GetPosition().x + m_Context->GetSize().x; x += grid_step) {
    //     DrawLine(x, m_Context->GetPosition().y, x, m_Context->GetPosition().y + m_Context->GetSize().y, Color{200, 200, 200, 255});
    // }
    // for (float y = m_Context->GetPosition().y; y < m_Context->GetPosition().y + m_Context->GetSize().y; y += grid_step) {
    //     DrawLine(m_Context->GetPosition().x, y, m_Context->GetPosition().x + m_Context->GetSize().x, y, Color{200, 200, 200, 255});
    // }
    m_Renderer.Render(*m_SceneContext, m_Camera);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_NoScrollbar);
    {
        ImGui::PopStyleVar();

        // Gather viewport info
        m_Data.IsFocused = ImGui::IsWindowFocused();
        m_Data.IsHovered = ImGui::IsWindowHovered();

        auto viewport_shift      = ImGui::GetWindowPos();
        auto viewport_min_region = ImGui::GetWindowContentRegionMin();
        auto viewport_max_region = ImGui::GetWindowContentRegionMax();

        auto viewport_size       = viewport_max_region - viewport_min_region;
        auto viewport_position   = viewport_shift + viewport_min_region;
        auto mouse_position      = ImGui::GetMousePos() - viewport_position;
        
        m_Data.Size           = { viewport_size.x, viewport_size.y };
        m_Data.Position       = { viewport_position.x, viewport_position.y };
        m_Data.MousePosition  = { mouse_position.x, mouse_position.y };
        m_Data.MouseInBounds  = m_Data.MousePosition.x >= 0 && m_Data.MousePosition.y >= 0
                             && m_Data.MousePosition.x <= m_Data.Size.x && m_Data.MousePosition.y <= m_Data.Size.y;
        
        Rectangle view_rect = { 0.0f, 0.0f, m_Data.Size.x, -m_Data.Size.y };

        rlImGuiImageRect(m_Renderer.GetView(), (int)m_Data.Size.x, (int)m_Data.Size.y, view_rect);
        // DrawGrid();

        if (m_SelectionTool.IsActive())
            m_SelectionTool.OnRender();
        if (m_CreationTool.IsActive())
            m_CreationTool.OnRender();

        if (m_ShowOverlay)
            DebugPanel(&m_ShowOverlay);

        if (m_ShowTools)
            Toolbar(&m_ShowTools);

        if (m_ShowPicker)
            ColorPicker(&m_ShowPicker);
    }
    
    ImGui::End();
}

void ViewportWindow::OnShutdown() {
    
}

void ViewportWindow::HandleInput() {
    if (IsKeyPressed(KEY_GRAVE))
        m_ShowOverlay = !m_ShowOverlay;
    if (IsKeyPressed(KEY_T))
        m_ShowTools = !m_ShowTools;

    if (IsMouseButtonPressed(MOUSE_BUTTON_MIDDLE))
        SetMouseCursor(MOUSE_CURSOR_RESIZE_ALL);
    if (IsMouseButtonReleased(MOUSE_BUTTON_MIDDLE))
        SetMouseCursor(MOUSE_CURSOR_DEFAULT);

    // TODO: use IsKeyDown with timer
    if (m_SceneContext->HasSelectedEntities()) {
        if (IsKeyPressed(KEY_RIGHT_BRACKET))
            m_SceneContext->PushEntityBack(m_SceneContext->GetSelectedEntity());
        else if (IsKeyPressed(KEY_LEFT_BRACKET))
            m_SceneContext->PushEntityFront(m_SceneContext->GetSelectedEntity());
    }
}

void ViewportWindow::HandleViewportResize() {
    if (m_Data.Size.x > 0.0f && m_Data.Size.y > 0.0f) {
        if (!Vector2Equals(m_Data.Size, m_Data.InitialSize)) {
            m_Data.InitialSize = m_Data.Size;
            m_Data.Resized = true;
        }
        else {
            m_Data.Resized = false;
        }
    }
}

/************************************************************
****                      PANELS                         ****
*************************************************************/

void ViewportWindow::DebugPanel(bool* open) {
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoScrollbar;
    
    ImGui::SetNextWindowPos({ m_Data.Position.x + m_Data.Size.x - 6, m_Data.Position.y + 6} , ImGuiCond_Always, { 1.0f, 0.0f });
    ImGui::SetNextWindowBgAlpha(0.35f);
    
    if (ImGui::BeginChild("Debug", { 360.0f, 0.0f }, ImGuiChildFlags_AutoResizeX | ImGuiChildFlags_AutoResizeY, window_flags))
    {
        ImGui::SeparatorText("Scene");
        Entity selected_entity = m_SceneContext->GetSelectedEntity();
        ImGui::Text("   Selected entity: %s (%u)", ((entt::entity)selected_entity != entt::null ? selected_entity.GetTag().c_str() : "None"), (uint32_t)(m_SceneContext->GetSelectedEntity()));

        auto bool_to_str = [this](bool value) { return value ? "true" : "false"; };
        ImGui::SeparatorText("Viewport data");
        Vector2 mouse_world_pos = GetScreenToWorld2D(m_Data.MousePosition, m_Camera);

        ImGui::Text("   Size: (%.1f,%.1f)",                 m_Data.Size.x, m_Data.Size.y);
        ImGui::Text("   Position: (%.1f,%.1f)",             m_Data.Position.x, m_Data.Position.y);
        ImGui::Text("   Mouse position: (%.1f,%.1f)",       m_Data.MousePosition.x, m_Data.MousePosition.y);
        ImGui::Text("   Mouse world position: (%.1f,%.1f)", mouse_world_pos.x, mouse_world_pos.y);
        ImGui::Text("   Is hovered: %s",                    bool_to_str(m_Data.IsHovered)            );
        ImGui::Text("   Is focused: %s",                    bool_to_str(m_Data.IsFocused)            );
        ImGui::Text("   Is toolbar hovered: %s",            bool_to_str(m_Data.IsToolbarHovered)     );
        ImGui::Text("   Is color picker hovered: %s",       bool_to_str(m_Data.IsColorPickerHovered) );
        ImGui::Text("   Mouse in bounds: %s",               bool_to_str(m_Data.MouseInBounds)        );
        ImGui::Text("   Resized: %s",                       bool_to_str(m_Data.Resized)              );

        ImGui::SeparatorText("Camera");
        auto camera = (Camera2D)m_Camera;
        ImGui::Text("   Offset: (%.1f,%.1f)", camera.offset.x, camera.offset.y);
        ImGui::Text("   Target: (%.1f,%.1f)", camera.target.x, camera.target.y);
        ImGui::Text("   Rotation: %.1f",      camera.rotation                 );
        ImGui::Text("   Zoom: %.1f",          camera.zoom                     );
        
        if (m_CreationTool.IsActive()) {
            const char* primitive_name[4] = { "None", "Line", "Rectangle", "Ellipse" };
            std::string label = std::string("Creation tool: ") + primitive_name[(size_t)m_CreationTool.GetPrimitiveType()];
            ImGui::SeparatorText(label.c_str());
            auto& p0 =  m_CreationTool.GetInitialPosition();
            auto& p1 =  m_CreationTool.GetCurrentPosition();
            ImGui::Text("   Is drawing: %s", bool_to_str(m_CreationTool.IsDrawing()));
            ImGui::Text("   Region: (%.1f, %.1f) : (%.1f, %.1f)", p0.x, p0.y, p1.x, p1.y);
        }
        else if (m_SelectionTool.IsActive()) {
            ImGui::SeparatorText("Selection tool");
            auto& p0 =  m_SelectionTool.GetInitialPosition();
            auto& p1 =  m_SelectionTool.GetCurrentPosition();
            ImGui::Text("   Is selecting region: %s", bool_to_str(m_SelectionTool.IsSelectingRegion()));
            ImGui::Text("   Region: (%.1f, %.1f) : (%.1f, %.1f)", p0.x, p0.y, p1.x, p1.y);
        }

        if (ImGui::BeginPopupContextWindow()) {
            if (open && ImGui::MenuItem("Close")) *open = false;
            ImGui::EndPopup();
        }
    }
    ImGui::EndChild();
}

void ViewportWindow::Toolbar(bool* open) {
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground;

    const float padding = 8.0f;
    ImGui::SetNextWindowPos({ m_Data.Position.x + padding, m_Data.Position.y + padding } , ImGuiCond_Always);
    window_flags |= ImGuiWindowFlags_NoMove;
    ImGui::SetNextWindowSize({40.0f, 0.0f});
    ImGui::SetNextWindowSizeConstraints({0.0f, 0.0f}, {40.0f, FLT_MAX});
    
    if (ImGui::BeginChild("Tool buttons", { 0.0f, 0.0f }, ImGuiChildFlags_AutoResizeY, window_flags))
    {
        m_Data.IsToolbarHovered = ImGui::IsWindowHovered();
        m_Data.IsHovered = m_Data.IsHovered && !m_Data.IsToolbarHovered;

        const int tool_count = 4;
        const char* tool_titles[tool_count]   = { "##Select##", "##Line##", "##Rectangle##", "##Ellipse##" };
        const Texture* tool_icons[tool_count] = { &m_SelectionIcon, &m_LineIcon, &m_RectangleIcon, &m_EllipseIcon };
        // const int group_sizes[2] = { 1, 3 };

        static int selected_tool = 0;

        // ImGui::ImageButton((ImTextureID)selection_icon.id, {40.0f, 40.0f});
        // rlImGuiImageButtonSize("##button##", &selection_icon, {40.0f, 40.0f});

        // ImGui::SetCurrentContext(GlobalContext);

        // ImGui::SetCurrentContext(
   
        // ImGui::ImageButton("##button##", (ImTextureID)selection_icon.id, {40.0f, 40.0f});

        // disable frame padding
        for (int current_tool = 0; current_tool < tool_count; ++current_tool) {
            bool tool_selected = current_tool == selected_tool;
            if (tool_selected) {
                ImGui::PushStyleColor(ImGuiCol_Button,        {0.3f, 0.4f, 0.9f, 1.0f});
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, {0.3f, 0.4f, 0.9f, 1.0f});
            }

            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, {4.0f, 4.0f});
            if (rlImGuiImageButtonSize(tool_titles[current_tool], tool_icons[current_tool], {32.0f, 32.0f})) {
                

                selected_tool = current_tool;

                if (current_tool == 0) {
                    m_SelectionTool.SetActive(true);
                    m_CreationTool.SetActive(false);
                }
                else if (current_tool == 1) {
                    m_CreationTool.SetActive(true);
                    m_SelectionTool.SetActive(false);
                    m_CreationTool.SetPrimitiveType(Tool::PrimitiveType::Line);
                }
                else if (current_tool == 2) {
                    m_CreationTool.SetActive(true);
                    m_SelectionTool.SetActive(false);
                    m_CreationTool.SetPrimitiveType(Tool::PrimitiveType::Rectangle);
                }
                else if (current_tool == 3) {
                    m_CreationTool.SetActive(true);
                    m_SelectionTool.SetActive(false);
                    m_CreationTool.SetPrimitiveType(Tool::PrimitiveType::Ellipse);
                }
            }
            if (tool_selected)
                ImGui::PopStyleColor(2);
            ImGui::PopStyleVar();
        }
    }
    ImGui::EndChild();
}

void ViewportWindow::ColorPicker(bool* show) {
    ImGui::SetNextWindowPos({ m_Data.Position.x + 6, m_Data.Position.y + m_Data.Size.y - 6} , ImGuiCond_Always, { 0.0f, 1.0f });
    // ImGui::SetNextWindowBgAlpha(0.5f);

    // ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.3f, 0.5f, 0.9f, 1.0f));
    // ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding())
    if (ImGui::BeginChild("ColorPicker", { 40.0f, 40.0f }, ImGuiChildFlags_None, ImGuiWindowFlags_NoBackground)) {
    // ImGui::PopStyleColor();

    static ImVec4 color = ImVec4(114.0f / 255.0f, 144.0f / 255.0f, 154.0f / 255.0f, 200.0f / 255.0f);

    // Generate a default palette. The palette will persist and can be edited.
    static bool saved_palette_init = true;
    static ImVec4 saved_palette[32] = {};
    if (saved_palette_init)
    {
        for (int n = 0; n < IM_ARRAYSIZE(saved_palette); n++)
        {
            ImGui::ColorConvertHSVtoRGB(n / 31.0f, 0.8f, 0.8f,
                saved_palette[n].x, saved_palette[n].y, saved_palette[n].z);
            saved_palette[n].w = 1.0f; // Alpha
        }
        saved_palette_init = false;
    }

    static ImVec4 backup_color;
    bool open_popup = ImGui::ColorButton("MyColor##3b", color, ImGuiColorEditFlags_AlphaPreview, {40.0f, 40.0f});
    ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
    // open_popup |= ImGui::Button("Palette");
    if (open_popup)
    {
        ImGui::OpenPopup("mypicker");
        backup_color = color;
    }
    if (ImGui::BeginPopup("mypicker"))
    {
        if (ImGui::ColorPicker4("##picker", (float*)&color, ImGuiColorEditFlags_PickerHueWheel | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview)) {
            m_CreationTool.SetPrimitiveColor(rlImGuiColors::Convert(color));
        }
        ImGui::SameLine();

        ImGui::BeginGroup(); // Lock X position
        ImGui::Text("Current");
        ImGui::ColorButton("##current", color, ImGuiColorEditFlags_NoBorder | ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_AlphaPreviewHalf, ImVec2(60, 40));
        ImGui::Text("Previous");
        if (ImGui::ColorButton("##previous", backup_color, ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_AlphaPreviewHalf, ImVec2(60, 40)))
            color = backup_color;
        ImGui::Separator();
        ImGui::Text("Palette");
        for (int n = 0; n < IM_ARRAYSIZE(saved_palette); n++)
        {
            ImGui::PushID(n);
            if ((n % 8) != 0)
                ImGui::SameLine(0.0f, ImGui::GetStyle().ItemSpacing.y);

            ImGuiColorEditFlags palette_button_flags = ImGuiColorEditFlags_NoBorder | ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoTooltip;
            if (ImGui::ColorButton("##palette", saved_palette[n], palette_button_flags, ImVec2(20, 20)))
                color = ImVec4(saved_palette[n].x, saved_palette[n].y, saved_palette[n].z, color.w); // Preserve alpha!

            // Allow user to drop colors into each palette entry. Note that ColorButton() is already a
            // drag source by default, unless specifying the ImGuiColorEditFlags_NoDragDrop flag.
            if (ImGui::BeginDragDropTarget())
            {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_3F))
                    memcpy((float*)&saved_palette[n], payload->Data, sizeof(float) * 3);
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_4F))
                    memcpy((float*)&saved_palette[n], payload->Data, sizeof(float) * 4);
                ImGui::EndDragDropTarget();
            }

            ImGui::PopID();
        }
        ImGui::EndGroup();
        ImGui::EndPopup();
    }

    };
    ImGui::EndChild();
}

void ViewportWindow::DrawGrid() {
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    Camera2D camera = (Camera2D)m_Camera;
    auto canvas_sz = m_Data.Size;
    auto canvas_p0 = m_Data.Position;
    auto canvas_p1 = Vector2Add(canvas_sz, canvas_p0);
    auto scrolling = Vector2Scale(Vector2Add(GetWorldToScreen2D(camera.offset, camera), camera.target), camera.zoom);

    // draw_list->PushClipRect(canvas_p0, canvas_p1, true);
    const float GRID_STEP = 64.0f * camera.zoom;
    for (float x = fmodf(scrolling.x, GRID_STEP); x < canvas_sz.x; x += GRID_STEP)
        draw_list->AddLine(ImVec2(canvas_p0.x + x, canvas_p0.y), ImVec2(canvas_p0.x + x, canvas_p1.y), IM_COL32(200, 200, 200, 40));
    for (float y = fmodf(scrolling.y, GRID_STEP); y < canvas_sz.y; y += GRID_STEP)
        draw_list->AddLine(ImVec2(canvas_p0.x, canvas_p0.y + y), ImVec2(canvas_p1.x, canvas_p0.y + y), IM_COL32(200, 200, 200, 40));
    // draw_list->PopClipRect();
}

}