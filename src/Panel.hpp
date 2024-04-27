#pragma once

#include "RaylibObject.hpp"
#include "Core.hpp"

#include <raylib.h>
#include <vector>

namespace cw {

class Scene;
class PanelManager;

class Panel : public RaylibObject {
private:
    friend PanelManager;

public:
    virtual ~Panel() = default;

    virtual void OnSceneLoad(Scene* scene) = 0;
    virtual void OnSceneUnload() = 0;
};

class PanelManager : public RaylibObject {
public:
    void PushPanel(Ref<Panel> panel);
    void PushPanels(std::initializer_list<Ref<Panel>> panels);
    void PopPanel();

    virtual void OnSetup() override;
    virtual void OnUpdate() override;
    virtual void OnRender() override;
    virtual void OnShutdown() override;

    virtual void OnSceneLoad(Scene* scene);
    virtual void OnSceneUnload();

private:
    std::vector<Ref<Panel>> m_Panels;
};

}
