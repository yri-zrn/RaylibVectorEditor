#pragma once

#include "SceneRenderer.hpp"
#include "ViewportCollisionResolver.hpp"
#include "ViewportCamera.hpp"

#include "Panel.hpp"
#include "Scene.hpp"
#include "Entity.hpp"
#include "Tool.hpp"

namespace cw {

class Scene;

struct ViewportData {
    bool    IsHovered            = false;
    bool    IsFocused            = false;
    bool    IsToolbarHovered     = false;
    bool    IsColorPickerHovered = false;
    bool    MouseInBounds        = false;
    bool    Resized              = true;
    Vector2 InitialSize          = { 0 };
    Vector2 Size                 = { 0 };
    Vector2 MousePosition        = { 0 };
    Vector2 Position             = { 0 };
};

class ViewportWindow : public Panel {
private:
    friend SceneSerializer;

public:
    ViewportWindow() = default;
    virtual ~ViewportWindow() = default;

    virtual void OnSetup()    override;
    virtual void OnUpdate()   override;
    virtual void OnRender()   override;
    virtual void OnShutdown() override;

    virtual void OnSceneLoad(Scene* scene) override { 
        m_SceneContext = scene;
        m_Renderer.OnSceneLoad(scene);
    }
    virtual void OnSceneUnload() override {
        m_Renderer.OnSceneUnload();
    }

    bool IsHovered() const { return m_Data.IsHovered; }
    bool IsFocused() const { return m_Data.IsFocused; }
    bool IsToolbarHovered() const { return m_Data.IsToolbarHovered; }
    bool Resized() const { return m_Data.Resized; }
    Vector2 GetSize() const { return m_Data.Size; }
    Vector2 GetMousePosition() const { return m_Data.MousePosition; }
    Vector2 GetPosition() const { return m_Data.Position; }
    Camera2D GetCamera() const { return (Camera2D)m_Camera; }
    Scene* GetScene() { return m_SceneContext; }

    const SceneRenderer& GetRenderer() const { return m_Renderer; }
    const ViewportCollisionResolver& GetCollisionResolver() const { return m_CollisionResolver; }

private:
    void HandleInput();
    void HandleViewportResize();

private:
    void DebugPanel(bool* show);
    void Toolbar(bool* show);
    void ColorPicker(bool* show);

private:
    void DrawGrid();

private:
    ViewportData m_Data;
    Scene*       m_SceneContext;

    bool m_ShowOverlay = true;
    bool m_ShowPicker  = true;
    bool m_ShowTools   = true;

    SceneRenderer          m_Renderer;
    ViewportCollisionResolver m_CollisionResolver;
    ViewportCamera            m_Camera;

    CreationTool  m_CreationTool;
    SelectionTool m_SelectionTool;

    Texture m_SelectionIcon;
    Texture m_LineIcon;
    Texture m_RectangleIcon;
    Texture m_EllipseIcon;
};

}