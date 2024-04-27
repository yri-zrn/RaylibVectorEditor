#pragma once

#include "RaylibObject.hpp"
#include "Scene.hpp"
#include "Entity.hpp"

namespace cw {

class ViewportWindow;

class ViewportCamera;

class SceneRenderer : public RaylibObject {
public:
    SceneRenderer()  = default;
    ~SceneRenderer() = default;

    void SetContext(ViewportWindow* context) { m_Context = context; }
    Texture* GetView() { return &m_View.texture; }

    void Render(Scene& scene, const ViewportCamera& camera);
    void Draw(Entity entity);
    
    virtual void OnSetup()    override;
    virtual void OnUpdate()   override;
    virtual void OnShutdown() override;

    void OnSceneLoad(Scene* scene);
    void OnSceneUnload();

    void OnPrimitiveAdded(Entity& entity);

private:
    void HandleViewportResize(const Vector2& size);

private:
    RenderTexture2D m_RawRender;
    RenderTexture2D m_View;

    Scene* m_SceneContext;
    ViewportWindow* m_Context;

    Shader m_FXAA_Shader;
};

};