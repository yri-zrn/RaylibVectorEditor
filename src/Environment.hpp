#pragma once

#include "Core.hpp"
#include "RaylibObject.hpp"
#include "Panel.hpp"

#include "Scene.hpp"
#include "ViewportWindow.hpp"
#include "HierarchyWindow.hpp"
#include "PropertiesWindow.hpp"

namespace cw {

class Environment : public RaylibObject {
public:
    Environment() = default;
    ~Environment() = default;

    virtual void OnSetup() override;
    virtual void OnUpdate() override;
    virtual void OnRender() override;
    virtual void OnShutdown() override;

    void SaveScene();
    void LoadScene();
    void UnloadScene();
    void OnSceneLoad();
    void OnSceneUnload();
    
#ifdef TEST_WINDOW
    void ShowTestWindow();
    void TestWindow2(bool* open);
    
    template<typename Func>
    void SpacedGroup(Func panel_func, ImVec2 spacing = { 8.0f, 4.0f }) {
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, spacing);
        ImGui::BeginGroup();
        panel_func();
        ImGui::PopStyleVar();
        ImGui::EndGroup();
    }

    template<typename Func>
    void Group(Func panel_func) {
        SpacedGroup(panel_func, { 0.0f, 0.0f });
    }
#endif

private:
    void HandleKeyboardInput();

private:
    void SetTheme();
    void SetDarkColorTheme();

private:
    Ref<Scene> m_Scene;

    PanelManager m_PanelManager;

    bool m_Debug = true;
};

}
