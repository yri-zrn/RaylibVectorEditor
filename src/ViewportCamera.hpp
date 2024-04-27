#pragma once

#include "RaylibObject.hpp"

namespace cw {

class ViewportWindow;

class ViewportCamera : public RaylibObject {
public:
    ViewportCamera() = default;
    ~ViewportCamera() = default;

    void SetContext(ViewportWindow* context) { m_Context = context; }

    virtual void OnSetup() override;
    virtual void OnUpdate() override;

    operator Camera2D() const { return m_Camera; }

    template<class Archive>
    friend void serialize(Archive& archive, ViewportCamera& camera);

private:
    void HandleViewportResize(const Vector2& size);

private:
    void CameraMove(Vector2 move_delta);
    void CameraZoom(Vector2 mouse_position, float zoom_delta);

private:
    Camera2D m_Camera;

    ViewportWindow* m_Context;
};


}