#pragma once

#include "Panel.hpp"

#include "Scene.hpp"

namespace cw {

class PropertiesWindow : public Panel {
public:
    virtual ~PropertiesWindow() = default;

    virtual void OnSetup() override;
    virtual void OnUpdate()override;
    virtual void OnRender() override;
    virtual void OnShutdown() override;

    virtual void OnSceneLoad(Scene* scene) override { m_Context = scene; }
    virtual void OnSceneUnload() override { }

private:
    void Tag(Entity& entity);
    
    template<typename Component, typename Func>
    void PropertyNode(Entity& entity, Component& component, std::string node_name, bool removable = true, Func widget_func = 0);

    template<typename Component>
    void Property(Entity& entity);
    
    template<> void Property<ColorComponent>(Entity& entity);
    template<> void Property<LineComponent>(Entity& entity);
    template<> void Property<RectangleComponent>(Entity& entity);
    template<> void Property<EllipseComponent>(Entity& entity);

private:
    Scene* m_Context;
};

}