#include "ViewportCamera.hpp"

#include "ViewportWindow.hpp"

namespace cw {

void ViewportCamera::OnSetup() {
    m_Camera.zoom     = 1.0f;
    m_Camera.target.x = 0.0f;
    m_Camera.target.y = 0.0f;
    m_Camera.rotation = 0.0f;
    m_Camera.offset.x = GetScreenWidth() / 2.0f;
    m_Camera.offset.y = GetScreenHeight() / 2.0f;
}

void ViewportCamera::OnUpdate() {
    if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE)) {
        CameraMove(GetMouseDelta());
    }
    if (m_Context->IsHovered()) {
        float wheel_delta = GetMouseWheelMove();
        if (wheel_delta != 0)
            CameraZoom(m_Context->GetMousePosition(), wheel_delta);
    }

    if (m_Context->Resized())
        HandleViewportResize(m_Context->GetSize());
}

void ViewportCamera::HandleViewportResize(const Vector2& size) {
    Vector2 viewport_center = {0.0f, 0.0f};
    Vector2 viewport_center_world_pos = GetScreenToWorld2D(viewport_center, m_Camera);

    m_Camera.offset = viewport_center;
    m_Camera.target = viewport_center_world_pos;
}

void ViewportCamera::CameraMove(Vector2 move_delta) {
    // Vector2 delta = GetMouseDelta();
    move_delta = Vector2Scale(move_delta, -1.0f/m_Camera.zoom);
    m_Camera.target = Vector2Add(m_Camera.target, move_delta);
}

void ViewportCamera::CameraZoom(Vector2 mouse_position, float zoom_delta) {
    // Get the world point that is under the mouse
    Vector2 mouseWorldPos = GetScreenToWorld2D(mouse_position, m_Camera);
    
    // Set the offset to where the mouse is
    // m_Camera.offset = {m_Context->GetSize().x/2.0f + mouse_position.x, m_Context->GetSize().y/2.0f + mouse_position.y};
    // m_Camera.offset = { mouse_position.x * 0.5f, mouse_position.y * 0.5f };
    m_Camera.offset = mouse_position;

    // Set the target to match, so that the camera maps the world space point
    // under the cursor to the screen space point under the cursor at any zoom
    m_Camera.target = mouseWorldPos;

    // Zoom increment
    const float zoomIncrement = 0.125f;

    m_Camera.zoom += zoom_delta * zoomIncrement;
    if (m_Camera.zoom < zoomIncrement) m_Camera.zoom = zoomIncrement;
}

}