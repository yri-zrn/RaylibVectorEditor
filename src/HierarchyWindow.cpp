#include "HierarchyWindow.hpp"

#include <imgui.h>

namespace cw {

void HierarchyWindow::OnSetup() {

}

void HierarchyWindow::OnUpdate() {

}

void HierarchyWindow::OnRender() {
    ImGui::Begin("Scene");
    {
        m_Context->ForEach([&](auto entity_handle) {
            Entity entity{ entity_handle, m_Context };
            EntityNode(entity);
        });
    }

    if (ImGui::IsMouseClicked(0) && ImGui::IsWindowHovered()) {
        m_Context->DeselectEntities();
    }

    if (ImGui::BeginPopupContextWindow("CreateEntity", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems)) {
        if (ImGui::MenuItem("Create Entity"))
            m_Context->CreateEntity();

        ImGui::EndPopup();
    }

    ImGui::End();

    // ImGui::Begin("Properties");
    // {
    //     if (m_SelectionContext) {
    //         DrawComponents(m_SelectionContext);

    //         if (ImGui::Button("Add component")) {
    //             ImGui::OpenPopup("AddComponent");
    //         }

    //         if (ImGui::BeginPopup("AddComponent")) {
    //             if (ImGui::MenuItem("Camera")) {
    //                 m_SelectionContext.AddComponent<CameraComponent>();
    //                 ImGui::CloseCurrentPopup();
    //             }

    //             if (ImGui::MenuItem("Mesh renderer")) {
    //                 m_SelectionContext.AddComponent<MeshRendererComponent>();
    //                 ImGui::CloseCurrentPopup();
    //             }

    //             if (ImGui::MenuItem("Point Light")) {
    //                 m_SelectionContext.AddComponent<PointLightComponent>();
    //                 ImGui::CloseCurrentPopup();
    //             }

    //             if (ImGui::MenuItem("Directional Light")) {
    //                 m_SelectionContext.AddComponent<DirectionalLightComponent>();
    //                 ImGui::CloseCurrentPopup();
    //             }

    //             ImGui::EndPopup();
    //         }
    //     }
    // }
    // ImGui::End();
}

void HierarchyWindow::OnShutdown() {

}

void HierarchyWindow::EntityNode(Entity& entity) {
    Entity selected_entity = m_Context->GetSelectedEntity();

    auto& tag = entity.GetComponent<TagComponent>().Tag;

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
    if (selected_entity == entity) flags |= ImGuiTreeNodeFlags_Selected;
    
    bool opened = ImGui::TreeNodeEx((void*)(uint64_t)((uint32_t)entity), flags, "%s", tag.c_str());
    if (ImGui::IsItemClicked()) {
        m_Context->SelectEntity(entity);
    }

    bool entity_deleted = false;
    if (ImGui::BeginPopupContextItem()) {
        if (ImGui::MenuItem("Delete entity"))
            entity_deleted = true;
        ImGui::EndPopup();
    }

    if (opened) {
        ImGui::TreePop();
    }

    if (entity_deleted) {
        m_Context->DestroyEntity(entity);
        if (m_Context->GetSelectedEntity() == entity)
            m_Context->SelectEntity(entity);
    }
}

}
