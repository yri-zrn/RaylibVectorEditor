#include "Environment.hpp"

#include "RaylibApplication.hpp"
#include "SceneSerializer.hpp"

#include <nfd.h>
#include <iostream>

namespace cw {

static void ShowPlaceholderObject(const char* prefix, int uid)
{
    // Use object uid as identifier. Most commonly you could also use the object pointer as a base ID.
    ImGui::PushID(uid);

    // Text and Tree nodes are less high than framed widgets, using AlignTextToFramePadding() we add vertical spacing to make the tree lines equal high.
    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    ImGui::AlignTextToFramePadding();
    bool node_open = ImGui::TreeNode("Object", "%s_%u", prefix, uid);
    ImGui::TableSetColumnIndex(1);
    ImGui::Text("my sailor is rich");

    if (node_open)
    {
        static float placeholder_members[8] = { 0.0f, 0.0f, 1.0f, 3.1416f, 100.0f, 999.0f };
        for (int i = 0; i < 8; i++)
        {
            ImGui::PushID(i); // Use field index as identifier.
            if (i < 2)
            {
                ShowPlaceholderObject("Child", 424242);
            }
            else
            {
                // Here we use a TreeNode to highlight on hover (we could use e.g. Selectable as well)
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::AlignTextToFramePadding();
                ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet;
                ImGui::TreeNodeEx("Field", flags, "Field_%d", i);

                ImGui::TableSetColumnIndex(1);
                ImGui::SetNextItemWidth(-FLT_MIN);
                if (i >= 5)
                    ImGui::InputFloat("##value", &placeholder_members[i], 1.0f);
                else
                    ImGui::DragFloat("##value", &placeholder_members[i], 0.01f);
                ImGui::NextColumn();
            }
            ImGui::PopID();
        }
        ImGui::TreePop();
    }
    ImGui::PopID();
}

void Environment::OnSetup() {
    m_Scene = CreateRef<Scene>();

    m_PanelManager.PushPanel(CreateRef<ViewportWindow>());
    m_PanelManager.PushPanel(CreateRef<HierarchyWindow>());
    m_PanelManager.PushPanel(CreateRef<PropertiesWindow>());

    m_PanelManager.OnSceneLoad(m_Scene.get());
    
    m_PanelManager.OnSetup();

    rlImGuiSetup(true);
    NFD_Init();

    SetTheme();
    SetDarkColorTheme();
}

void Environment::OnUpdate() {
    HandleKeyboardInput();

    m_PanelManager.OnUpdate();
}

void Environment::OnRender() {
	BeginDrawing();
    {
        ClearBackground(DARKGRAY);


        rlImGuiBegin();

        ImGuiIO& io = ImGui::GetIO();

        io.ConfigWindowsMoveFromTitleBarOnly = true;

        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

        const ImGuiViewport* imgui_viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(imgui_viewport->WorkPos);
        ImGui::SetNextWindowSize(imgui_viewport->WorkSize);
        ImGui::SetNextWindowViewport(imgui_viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
            window_flags |= ImGuiWindowFlags_NoBackground;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("DockSpace", nullptr, window_flags);
        ImGui::PopStyleVar();
        ImGui::PopStyleVar(2);

        // Submit the DockSpace
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
            ImGuiID dockspace_id = ImGui::GetID("DockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
        }

        static bool show_demo_window = true;

        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                ImGui::Separator();
                if (ImGui::MenuItem("Exit", "", false)) {
                    RaylibApplication::Get()->Close();
                }
                if (ImGui::MenuItem("Open", "", false)) {
                    LoadScene();
                }
                if (ImGui::MenuItem("Save", "", false)) {
                    SaveScene();
                }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Window"))
            {
                ImGui::MenuItem("ImGui Demo", nullptr, &show_demo_window);

                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        m_PanelManager.OnRender();

        ImGui::End();

        rlImGuiEnd();
    }
    EndDrawing();
}

void Environment::OnShutdown() {
    m_PanelManager.OnShutdown();
    rlImGuiShutdown();
    NFD_Quit();
}

void Environment::SaveScene() {
    nfdchar_t *outPath;
    nfdfilteritem_t filterItem[1] = { { "Scene file", "cwscene" } };
    nfdresult_t result = NFD_SaveDialog(&outPath, filterItem, 1, NULL, NULL);
    if (result == NFD_OKAY)
    {
        puts("Success!");
        puts(outPath);
        SceneSerializer::Serialize(std::string(outPath), *m_Scene);

        NFD_FreePath(outPath);
    }
    else if (result == NFD_CANCEL)
    {
        puts("User pressed cancel.");
    }
    else 
    {
        printf("Error: %s\n", NFD_GetError());
    }
}

void Environment::LoadScene() {
    nfdchar_t *outPath;
    nfdfilteritem_t filterItem[1] = { { "Scene file", "cwscene" } };
    nfdresult_t result = NFD_OpenDialog(&outPath, filterItem, 1, NULL);
    if (result == NFD_OKAY)
    {
        UnloadScene();

        SceneSerializer::Deserialize(std::string(outPath), *m_Scene);
        puts("Success!");
        puts(outPath);
        NFD_FreePath(outPath);

        OnSceneLoad();
    }
    else if (result == NFD_CANCEL)
    {
        puts("User pressed cancel.");
    }
    else 
    {
        printf("Error: %s\n", NFD_GetError());
    }
}

void Environment::UnloadScene() {
    m_Scene = CreateRef<Scene>();
    OnSceneUnload();
}

void Environment::OnSceneLoad() {
    m_PanelManager.OnSceneLoad(m_Scene.get());
    m_PanelManager.OnSetup();
}

void Environment::OnSceneUnload() {
    m_PanelManager.OnSceneUnload();
}


void Environment::HandleKeyboardInput() {
    if (IsKeyPressed(KEY_GRAVE)) {
        m_Debug = !m_Debug;
    }
}

void Environment::SetDarkColorTheme() {
    auto& colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_WindowBg] = ImVec4{ 0.1f, 0.105f, 0.11f, 1.0f };

    // Headers
    colors[ImGuiCol_Header] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
    colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
    colors[ImGuiCol_HeaderActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
    
    // Buttons
    colors[ImGuiCol_Button] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
    colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
    colors[ImGuiCol_ButtonActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

    // Frame BG
    colors[ImGuiCol_FrameBg] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
    colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
    colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

    // Tabs
    colors[ImGuiCol_Tab] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
    colors[ImGuiCol_TabHovered] = ImVec4{ 0.38f, 0.3805f, 0.381f, 1.0f };
    colors[ImGuiCol_TabActive] = ImVec4{ 0.28f, 0.2805f, 0.281f, 1.0f };
    colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };

    // Title
    colors[ImGuiCol_TitleBg] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
    colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

    // Docking
    // 72 25 237
    colors[ImGuiCol_DockingPreview] = ImVec4{ 0.2824, 0.098, 0.9294, 0.7 };
}

void Environment::SetTheme() {
    ImGuiStyle& style = ImGui::GetStyle();

    // Borders
    style.WindowBorderSize = 0.0f;
    style.ChildBorderSize  = 0.0f;
    style.PopupBorderSize  = 0.0f;
    style.FrameBorderSize  = 0.0f;
    style.TabBorderSize    = 0.0f;
    style.TabBarBorderSize = 0.0f;

    // Rounding
    style.WindowRounding    = 4.0f;
    style.ChildRounding     = 4.0f;
    style.FrameRounding     = 4.0f;
    style.PopupRounding     = 4.0f;
    style.ScrollbarRounding = 9.0f;
    style.GrabRounding      = 4.0f;
    style.TabRounding       = 4.0f;

    // Widgets
    style.WindowMenuButtonPosition = ImGuiDir_None;
}

#ifdef TEST_WINDOW
void Environment::TestWindow2(bool* open) {
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_AlwaysAutoResize;

    auto& style = ImGui::GetStyle();
    static int tab_count = 5;
    static int selected_tab = 0;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0.0f, 0.0f});
    ImGui::PushStyleColor(ImGuiCol_SeparatorHovered, style.Colors[ImGuiCol_ButtonActive]);
    ImGui::PushStyleColor(ImGuiCol_SeparatorActive, style.Colors[ImGuiCol_ButtonActive]);
    ImGui::Begin("window2", open, ImGuiWindowFlags_NoCollapse);
    ImGui::PopStyleVar();

        ImGui::PushStyleColor(ImGuiCol_TableBorderLight, style.Colors[ImGuiCol_ButtonActive]);
        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, {0.0f, 0.0f});
        if (ImGui::BeginTable("Tab", 2, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg)) {
        
        ImGui::PopStyleColor();

            ImGui::TableSetupColumn("AAA", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableSetupColumn("BBB", ImGuiTableColumnFlags_WidthStretch);
            
            // ImGui::TableHeadersRow();
            ImGui::TableNextColumn();
            
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0.0f, 0.0f});
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, {0.0f, 0.0f});
            if (ImGui::BeginChild("TabButtons", {0.0f, 0.0f}, ImGuiChildFlags_AutoResizeX | ImGuiChildFlags_AutoResizeY, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground)) {
            ImGui::PopStyleVar(2);
                for (int current_tab = 0; current_tab < tab_count; ++current_tab) {
                    std::string name = std::string("##Tab") + std::to_string(current_tab);
                    ImGui::PushID(name.c_str());
                    bool selected = selected_tab == current_tab;
                    if (selected) {
                        ImGui::PushStyleColor(ImGuiCol_Button, style.Colors[ImGuiCol_ButtonActive]);
                    }
                    

                    // ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
                    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, {0.0f, 0.0f});
                    if (ImGui::Button(name.c_str(), {32.0f, 32.0f}))
                        selected_tab = current_tab;
                    
                    ImGui::PopID();
                    
                    ImGui::PopStyleVar();
                    if (selected)
                        ImGui::PopStyleColor();
                }
                ImGui::EndChild();
            }
            
            ImGui::TableNextColumn();


            
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, {0.0f, 0.0f});
            if (ImGui::BeginChild("TabContents", {0.0f, 0.0f})) {
                ImGui::PopStyleVar();

                if (selected_tab == -1) {
                    ImGui::TextUnformatted("None");
                }
                else if (selected_tab == 0) {
                    ImGui::TextUnformatted("Hello world");
                }
                else if (selected_tab == 1) {
                    ImGui::TextUnformatted("Good morning");

                    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
                    if (ImGui::BeginTable("##split", 2, ImGuiTableFlags_BordersOuter | ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY))
                    {
                        ImGui::TableSetupScrollFreeze(0, 1);
                        ImGui::TableSetupColumn("Object");
                        ImGui::TableSetupColumn("Contents");
                        ImGui::TableHeadersRow();

                        // Iterate placeholder objects (all the same data)
                        for (int obj_i = 0; obj_i < 4; obj_i++)
                            ShowPlaceholderObject("Object", obj_i);

                        ImGui::EndTable();
                    }
                    ImGui::PopStyleVar();
                }
                else if (selected_tab == 2) {
                    ImGui::TextUnformatted("It'stimetosleepsssss");
                }
                ImGui::EndChild();
            }

            ImGui::PopStyleVar();
            ImGui::EndTable();
        }
    ImGui::PopStyleColor(2);
    ImGui::End();
    
    // if (ImGui::Begin("Test window2"), &open) {
    //     ImGui::TextUnformatted("HEEE!");
    //     ImGui::End();
    // }
}

void Environment::ShowTestWindow() {
    static int selected_tab = 0;

    auto& style = ImGui::GetStyle();

    auto tab_buttons = [style](){
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground;
        
        if (ImGui::BeginChild("TabButtons", {0.0f, 0.0f}, ImGuiChildFlags_None, window_flags)) {
            ImGui::SameLine(8.0f);
            int tab_count = 5;
            // ImGui::TextUnformatted("Hey");

            if (ImGui::BeginTable("TabButtonsTable", 1, ImGuiTableFlags_None, {0.0f, 0.0f})) {
                for (int current_tab = 0; current_tab < tab_count; ++current_tab) {
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    std::string name = std::string("##EditorTab") + std::to_string(current_tab);
                    ImGui::PushID(name.c_str());
                    bool selected = selected_tab == current_tab;
                    if (selected) {
                        ImGui::PushStyleColor(ImGuiCol_Button, style.Colors[ImGuiCol_ButtonActive]);
                        ImGui::PushStyleColor(ImGuiCol_Border, style.Colors[ImGuiCol_ButtonActive]);
                    }
                    
                    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.0f);

                    if (ImGui::Button(name.c_str(), {16.0f, 16.0f}))
                        selected_tab = current_tab;
                    
                    ImGui::PopStyleVar();
                    if (selected)
                        ImGui::PopStyleColor(2);
                    ImGui::PopID();
                }
                ImGui::EndTable();
            }

            ImGui::EndChild();
        }
    };

    auto tab_content = [style](){
        ImGui::PushStyleColor(ImGuiCol_ChildBg, style.Colors[ImGuiCol_WindowBg]);
        ImGui::PushStyleColor(ImGuiCol_Border, style.Colors[ImGuiCol_WindowBg]);
        if (ImGui::BeginChild("TabContent", {200.0f - 40.0f, 0.0f}, ImGuiChildFlags_None, ImGuiWindowFlags_NoDecoration)) {
            if (selected_tab == 0) {
                ImGui::TextUnformatted("Hey");
            }
            else if (selected_tab == 1) {
                ImGui::TextUnformatted("Hello what's up");
            }
            ImGui::PopStyleColor(2);
            ImGui::EndChild();
        }
    };
    
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f });
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0.0f, 0.0f });
    // ImGui::SetNextWindowSizeConstraints({0.0f, 0.0f}, {240.0f, FLT_MAX});
    ImGui::Begin("Test window");
    ImGui::PopStyleVar(2);

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0.0f, 0.0f });
    ImGui::BeginGroup();
    tab_buttons();
    ImGui::PopStyleVar();
    ImGui::EndGroup();

    ImGui::SameLine();
    
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0.0f, 0.0f });
    ImGui::BeginGroup();
    tab_content();
    ImGui::PopStyleVar();
    ImGui::EndGroup();
    
    
    // ImGui::TextUnformatted("Hey");

    
    ImGui::End();

    // const float padding = 10.0f;
    // const ImGuiViewport* viewport = ImGui::GetMainViewport();
    // ImGui::SetNextWindowPos({ m_ViewportPosition.x + padding, m_ViewportPosition.y + padding } , ImGuiCond_Always);
    // // ImGui::SetNextWindowViewport(viewport->ID);
    // window_flags |= ImGuiWindowFlags_NoMove;
    // ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background
    
    // ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 6.0f, 6.0f });
    // if (ImGui::Begin("Example: Simple overlay", open, window_flags))
    // {
    //     ImGui::Text("Camera:");
    //     ImGui::Text("   Offset: (%.1f,%.1f)", m_Camera.offset.x, m_Camera.offset.y);
    //     ImGui::Text("   Target: (%.1f,%.1f)", m_Camera.target.x, m_Camera.target.y);
    //     ImGui::Text("   Zoom:   %.1f", m_Camera.zoom);
    //     ImGui::Separator();
    //     ImGui::Text("Mouse position: (%.1f,%.1f)", m_ViewportMousePosition.x, m_ViewportMousePosition.y);

    //     if (ImGui::BeginPopupContextWindow())
    //     {
    //         if (open && ImGui::MenuItem("Close")) *open = false;
    //         ImGui::EndPopup();
    //     }
    // }
    // ImGui::PopStyleVar();
    // ImGui::End();
}
#endif

};