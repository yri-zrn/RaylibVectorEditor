#pragma once

#include "Panel.hpp"
#include "Scene.hpp"
#include "Entity.hpp"

namespace cw {

class HierarchyWindow : public Panel {
public:
    HierarchyWindow() = default;
    virtual ~HierarchyWindow() = default;

    virtual void OnSetup() override;
    virtual void OnUpdate() override;
    virtual void OnRender() override;
    virtual void OnShutdown() override;

    virtual void OnSceneLoad(Scene* scene) override { m_Context = scene; }
    virtual void OnSceneUnload() override { }

private:
    void EntityNode(Entity& entity);

private:
    Scene* m_Context;
};

};