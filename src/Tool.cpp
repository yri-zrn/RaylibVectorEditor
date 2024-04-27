#include "Tool.hpp"

#include "Scene.hpp"
#include "Entity.hpp"
#include "ViewportWindow.hpp"

#include <math.h>

namespace cw {

//******************************************
//****          Creation Tool           ****
//******************************************

void CreationTool::OnUpdate() {
    if (!m_Context.Viewport->IsHovered()) return;

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        if (m_Context.Viewport->IsHovered()) {
            m_InitialPosition = m_Context.Viewport->GetMousePosition();
            m_Drawing = true;
        }
    }
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        m_CurrentPosition = m_Context.Viewport->GetMousePosition();
    }
    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        if (m_Drawing) {
            CreateEntity();
            m_Drawing = false;
        }
    }
}

void CreationTool::OnRender() {
    if (!m_Drawing) return;

    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    float selection_thickness = 1.5f;
    uint32_t selection_color = IM_COL32(77, 102, 230, 230);

    auto p1 = Vector2Add(m_InitialPosition, m_Context.Viewport->GetPosition());
    auto p2 = Vector2Add(m_CurrentPosition, m_Context.Viewport->GetPosition());

    if (m_PrimitiveType == PrimitiveType::Line) {
        draw_list->AddLine({p1.x, p1.y}, {p2.x, p2.y}, selection_color, selection_thickness);
    }
    else if (m_PrimitiveType == PrimitiveType::Rectangle) {
        draw_list->AddRect({p1.x, p1.y}, {p2.x, p2.y}, selection_color, 0.0f, 0, selection_thickness);
    }
    else if (m_PrimitiveType == PrimitiveType::Ellipse) {
        ImVec2 center = { (p1.x + p2.x)/2.0f, (p1.y + p2.y)/2.0f };

        float radius_x = std::abs(p2.x - p1.x)/2.0f;
        float radius_y = std::abs(p2.y - p1.y)/2.0f;

        draw_list->AddEllipse(center, radius_x, radius_y, selection_color, 0.0f, 0, selection_thickness);
    }
}

void CreationTool::CreateEntity() {
    auto camera = m_Context.Viewport->GetCamera();
    Vector2 p1 = GetScreenToWorld2D({ m_InitialPosition.x, m_InitialPosition.y }, camera);
    Vector2 p2 = GetScreenToWorld2D({ m_CurrentPosition.x, m_CurrentPosition.y }, camera);

    if (Vector2Length(Vector2Subtract(p2, p1)) <= 2.0f) return;

    if (m_PrimitiveType == PrimitiveType::Line) {
        int line_count = m_Context.Scene->GetLineCount();
        std::string name = std::string("Line_") + std::to_string(line_count);

        auto entity = m_Context.Scene->CreateEntity(name.c_str());
        entity.GetComponent<ColorComponent>().Color = m_PrimitiveColor;

        entity.AddComponent<LineComponent>(p1, p2, 2.0f);
    }
    else if (m_PrimitiveType == PrimitiveType::Rectangle) {
        Vector2 left_upper = { std::min(p1.x, p2.x), std::min(p1.y, p2.y) };
        Vector2 right_lower = { std::max(p1.x, p2.x), std::max(p1.y, p2.y) };

        int rectangle_count = m_Context.Scene->GetRectangleCount();
        std::string name = std::string("Rectangle_") + std::to_string(rectangle_count);

        auto entity = m_Context.Scene->CreateEntity(name.c_str());
        entity.GetComponent<ColorComponent>().Color = m_PrimitiveColor;

        entity.AddComponent<RectangleComponent>(left_upper, right_lower);
    }
    else if (m_PrimitiveType == PrimitiveType::Ellipse) {
        Vector2 center = {
            (p1.x + p2.x)/2.0f,
            (p1.y + p2.y)/2.0f,
        };
        float radius_x = std::abs(p2.x - p1.x)/2.0f;
        float radius_y = std::abs(p2.y - p1.y)/2.0f;

        int ellipse_count = m_Context.Scene->GetEllipseCount();
        std::string name = std::string("Ellipse_") + std::to_string(ellipse_count);

        auto entity = m_Context.Scene->CreateEntity(name.c_str());
        entity.GetComponent<ColorComponent>().Color = m_PrimitiveColor;
        
        entity.AddComponent<EllipseComponent>(center, radius_x, radius_y);
    }
}

//******************************************
//****          Selection Tool          ****
//******************************************

void SelectionTool::OnUpdate() {
    if (!m_Context.Viewport->IsHovered()) return;

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        m_InitialPosition = m_Context.Viewport->GetMousePosition();
        m_StartedSelecting = true;
    }
    else if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        m_CurrentPosition = m_Context.Viewport->GetMousePosition();
        
        if (m_StartedSelecting && !m_SelectingRegion && Vector2Length(Vector2Subtract(m_CurrentPosition, m_InitialPosition)) >= 2.0f)
            m_SelectingRegion = true;
    }
    else if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        if (m_SelectingRegion) {
            auto& p1 = m_InitialPosition;
            auto& p2 = m_CurrentPosition;
            SelectRegion(Rectangle{ fminf(p1.x, p2.x), fminf(p1.y, p2.y), fabs(p1.x - p2.x), fabs(p1.y - p2.y) });
        }
        else
            SelectEntity(m_CurrentPosition);
        
        m_InitialPosition = {0};
        m_CurrentPosition = {0};
        m_SelectingRegion = false;
        m_StartedSelecting = false;
    }
}

void SelectionTool::OnRender() {
    DrawSelection();
    if (m_SelectingRegion && m_Context.Viewport->IsHovered())
        ShowSelectedRegion();
}

void SelectionTool::SelectEntity(const Vector2& mouse_position) {
    auto& scene = m_Context.Scene;
    auto& viewport = m_Context.Viewport;

    Entity found_entity = viewport->GetCollisionResolver().CheckCollision(mouse_position);

    scene->DeselectEntities();
    if (found_entity != Entity{entt::null})
        scene->SelectEntity(found_entity);
}

void SelectionTool::SelectRegion(const Rectangle& rec) {
    auto& scene = m_Context.Scene;
    auto& viewport = m_Context.Viewport;

    std::vector<Entity> found_entities = viewport->GetCollisionResolver().CheckCollision(rec);

    scene->DeselectEntities();
    for (auto entity : found_entities)
        scene->SelectEntity(entity);
}

void SelectionTool::DrawSelection() {
    if (!m_Context.Scene->HasSelectedEntities()) return;

    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    auto selected_entities = m_Context.Scene->GetSelectedEntities();
    auto camera = m_Context.Viewport->GetCamera();
    float selection_thickness = fmaxf(1.1f, 1.5f * camera.zoom);

    for (auto entity : selected_entities) {
        auto cc = entity.GetComponent<ColorComponent>().Color;
        uint32_t selection_color = IM_COL32(245, 155, 60, 230);

        if (entity.HasComponent<LineComponent>()) {
            auto& lc = entity.GetComponent<LineComponent>();
            auto p0 = Vector2Add(m_Context.Viewport->GetPosition(), GetWorldToScreen2D(lc.Begin, camera));
            auto p1 = Vector2Add(m_Context.Viewport->GetPosition(), GetWorldToScreen2D(lc.End, camera));
            float line_thickness = (lc.Thickness + 0.5f) * m_Context.Viewport->GetCamera().zoom;

            draw_list->AddLine({p0.x, p0.y}, {p1.x, p1.y}, selection_color, line_thickness);
        }
        else if (entity.HasComponent<RectangleComponent>()) {
            auto& rc = entity.GetComponent<RectangleComponent>();
            auto p0 = Vector2Add(m_Context.Viewport->GetPosition(), GetWorldToScreen2D(rc.Begin, camera));
            auto p1 = Vector2Add(m_Context.Viewport->GetPosition(), GetWorldToScreen2D(rc.End, camera));
            
            draw_list->AddRect({p0.x, p0.y}, {p1.x, p1.y}, selection_color, 0.0f, 0, selection_thickness);
        }
        else if (entity.HasComponent<EllipseComponent>()) {
            auto& ec = entity.GetComponent<EllipseComponent>();
            auto center = Vector2Add(m_Context.Viewport->GetPosition(), GetWorldToScreen2D(ec.Center, camera));
            float radius_x = ec.RadiusX * m_Context.Viewport->GetCamera().zoom;
            float radius_y = ec.RadiusY * m_Context.Viewport->GetCamera().zoom;

            draw_list->AddEllipse({center.x, center.y}, radius_x, radius_y, selection_color, 0.0f, 0, selection_thickness);
        }
    }
}

void SelectionTool::ShowSelectedRegion() {
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    
    auto p1 = Vector2Add(m_InitialPosition, m_Context.Viewport->GetPosition());
    auto p2 = Vector2Add(m_CurrentPosition, m_Context.Viewport->GetPosition());
    
    float selection_thickness = 1.5f;
    uint32_t selection_color = IM_COL32(245, 155, 60, 160);

    draw_list->AddRect({p1.x, p1.y}, {p2.x, p2.y}, selection_color, 0.0f, 0, selection_thickness);
}

}