#include "Panel.hpp"
#include "Scene.hpp"

namespace cw {

void PanelManager::PushPanel(Ref<Panel> panel) {
    m_Panels.push_back(panel);
}

void PanelManager::PushPanels(std::initializer_list<Ref<Panel>> panels) {
    // std::copy()
    // better use!
    for (auto panel : panels)
        m_Panels.push_back(panel);
}

void PanelManager::PopPanel() {
    m_Panels.pop_back();
}

void PanelManager::OnSetup() {
    for (auto& panel : m_Panels)
        panel->OnSetup();
}

void PanelManager::OnUpdate() {
    for (auto reversed_it = m_Panels.rbegin(); reversed_it != m_Panels.rend(); ++reversed_it) {
        auto panel = *reversed_it;
        panel->OnUpdate();
    }
}

void PanelManager::OnRender() {
    for (auto& panel : m_Panels)
        panel->OnRender();
}

void PanelManager::OnShutdown() {
    for (auto& panel : m_Panels)
        panel->OnShutdown();
}

void PanelManager::OnSceneLoad(Scene* scene) {
    for (auto& panel : m_Panels)
        panel->OnSceneLoad(scene);
}

void PanelManager::OnSceneUnload() {
    for (auto& panel : m_Panels)
        panel->OnSceneUnload();
}

}