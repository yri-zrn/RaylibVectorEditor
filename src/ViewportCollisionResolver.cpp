#include "ViewportCollisionResolver.hpp"

#include "ViewportWindow.hpp"
#include "SceneRenderer.hpp"

// raylib utils

static bool CheckBelongingRecs(Rectangle rec_in, Rectangle rec_out) {
    return rec_in.x >= rec_out.x && rec_in.y >= rec_out.y 
        && rec_in.x + rec_in.width <= rec_out.x + rec_out.width && rec_in.y + rec_in.height <= rec_out.y + rec_out.height;
}

static bool CheckBelongingLineRec(Vector2 p1, Vector2 p2, Rectangle rec) {
    return CheckBelongingRecs(Rectangle{ fminf(p1.x, p2.x), fminf(p1.y, p2.y), fabs(p1.x - p2.x), fabs(p1.y - p2.y)}, rec);
}

static bool CheckCollisionPointEllipse(Vector2 point, Vector2 center, float radiusH, float radiusV) {
    // Check bounding box first
    if (!CheckCollisionPointRec(point, Rectangle{center.x - radiusH, center.y - radiusV, 2.0f * radiusH, 2.0f * radiusV}))
        return false;
    
    float i = powf((point.x - center.x) / radiusH, 2.0f) + powf((point.y - center.y) / radiusV, 2.0f);
    return i <= 1.0f ? true : false;
}

static bool CheckCollisionEllipseRec(Vector2 center, float radiusH, float radiusV, Rectangle rec) {
    if (CheckBelongingRecs(Rectangle{center.x - radiusH, center.y - radiusV, 2.0f * radiusH, 2.0f * radiusV}, rec))
        return true;
    
    Vector2 nearest_corner_relative_pos;
    nearest_corner_relative_pos.x = fminf(fabs(center.x - rec.x), fabs(center.x - (rec.x + rec.width)));
    nearest_corner_relative_pos.y = fminf(fabs(center.y - rec.y), fabs(center.y - (rec.y + rec.height)));

    float distance_to_nearest_corner = Vector2Length(nearest_corner_relative_pos);
    if (distance_to_nearest_corner <= radiusH || distance_to_nearest_corner <= radiusV)
        return true;
    
    return false;
}

static bool CheckCollisionLineRec(Vector2 startPos, Vector2 endPos, Rectangle rec) {
    if (CheckBelongingLineRec(startPos, endPos, rec))
        return true;

    return CheckCollisionLines(startPos, endPos, {rec.x            , rec.y             }, {rec.x + rec.width, rec.y             }, nullptr)
        || CheckCollisionLines(startPos, endPos, {rec.x + rec.width, rec.y             }, {rec.x + rec.width, rec.y + rec.height}, nullptr)
        || CheckCollisionLines(startPos, endPos, {rec.x + rec.width, rec.y + rec.height}, {rec.x            , rec.y + rec.height}, nullptr)
        || CheckCollisionLines(startPos, endPos, {rec.x            , rec.y + rec.height}, {rec.x            , rec.y             }, nullptr);
}

namespace cw {

Entity ViewportCollisionResolver::CheckCollision(const Vector2& mouse_viewport_position) const {
    auto entity_stack = m_ViewportContext->GetScene()->GetEntityStack();

    Vector2 mouse_world_pos = GetScreenToWorld2D( mouse_viewport_position, m_ViewportContext->GetCamera());
    

    for (auto iter = entity_stack.rbegin(); iter != entity_stack.rend(); ++iter) {
        const Entity& entity = *iter;

        if (entity.HasComponent<LineComponent>()) {
            auto& lc = entity.GetComponent<LineComponent>();
            float selection_radius = 4.0f;
            float line_thickness = 2.0f + (selection_radius + lc.Thickness) * m_ViewportContext->GetCamera().zoom;
            if (CheckCollisionPointLine(mouse_world_pos, lc.Begin, lc.End, (int)(line_thickness))) {
                return entity;
            }
        }
        else if (entity.HasComponent<RectangleComponent>()) {
            auto& rc = entity.GetComponent<RectangleComponent>();
            Rectangle rect = Rectangle{rc.Begin.x, rc.Begin.y, rc.End.x - rc.Begin.x, rc.End.y - rc.Begin.y};
            if (CheckCollisionPointRec(mouse_world_pos, rect)) {
                return entity;
            }
        }
        else if (entity.HasComponent<EllipseComponent>()) {
            auto& ec = entity.GetComponent<EllipseComponent>();
            if (CheckCollisionPointEllipse(mouse_world_pos, ec.Center, ec.RadiusX, ec.RadiusY)) {
                return entity;
            }
        }
    }

    return Entity{ entt::null };
}

std::vector<Entity> ViewportCollisionResolver::CheckCollision(const Rectangle& rec) const {
    auto entity_stack = m_ViewportContext->GetScene()->GetEntityStack();
    
    std::vector<Entity> selected_entities;

    Vector2 rect_begin_world_pos = GetScreenToWorld2D(Vector2{rec.x, rec.y}, m_ViewportContext->GetCamera());
    Vector2 rect_end_world_pos   = GetScreenToWorld2D(Vector2{rec.x + rec.width, rec.y + rec.height}, m_ViewportContext->GetCamera());
    Rectangle selection_rect = { rect_begin_world_pos.x, rect_begin_world_pos.y, rect_end_world_pos.x - rect_begin_world_pos.x, rect_end_world_pos.y - rect_begin_world_pos.y };

    for (auto iter = entity_stack.rbegin(); iter != entity_stack.rend(); ++iter) {
        const Entity& entity = *iter;

        if (entity.HasComponent<LineComponent>()) {
            auto& lc = entity.GetComponent<LineComponent>();
            if (CheckCollisionLineRec(lc.Begin, lc.End, selection_rect)) {
                selected_entities.push_back(entity);
            }
        }
        else if (entity.HasComponent<RectangleComponent>()) {
            auto& rc = entity.GetComponent<RectangleComponent>();
            Rectangle rect = Rectangle{rc.Begin.x, rc.Begin.y, rc.End.x - rc.Begin.x, rc.End.y - rc.Begin.y};
            if (CheckCollisionRecs(rect, selection_rect)) {
                selected_entities.push_back(entity);
            }
        }
        else if (entity.HasComponent<EllipseComponent>()) {
            auto& ec = entity.GetComponent<EllipseComponent>();
            if (CheckCollisionEllipseRec(ec.Center, ec.RadiusX, ec.RadiusY, selection_rect)) {
                selected_entities.push_back(entity);
            }
        }
    }

    return selected_entities;
}

}