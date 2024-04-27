#include "PropertiesWindow.hpp"

#include "Entity.hpp"

#include <imgui.h>
#include <rlImGuiColors.h>
#include <string.h>

namespace cw {

void PropertiesWindow::OnSetup() { }
void PropertiesWindow::OnUpdate() { }

void PropertiesWindow::OnRender() {
    ImGui::Begin("PropertiesWindow", nullptr, ImGuiWindowFlags_None);
    if (m_Context->HasSelectedEntities()) {
        auto entity = m_Context->GetSelectedEntity();
        
        if (entity.HasComponent<TagComponent>())
            Tag(entity);

        if (entity.HasComponent<ColorComponent>())
            Property<ColorComponent>(entity);
        

        if (entity.HasComponent<LineComponent>()) {
            Property<LineComponent>(entity);
        }
        else if (entity.HasComponent<RectangleComponent>()) {
            Property<RectangleComponent>(entity);
        }
        else if (entity.HasComponent<EllipseComponent>()) {
            Property<EllipseComponent>(entity);
        }
    }
    ImGui::End();
}

void PropertiesWindow::OnShutdown() { }

// void PropertiesWindow::Properties(Entity& entity) {
    // if (entity.HasComponent<LineComponent>()) {
    //     bool open = ImGui::TreeNodeEx((void*)typeid(LineComponent).hash_code(), tree_node_flags, "%s", "Transform");

    //     ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4.0f, 4.0f });
    //     ImGui::SameLine(ImGui::GetWindowWidth() - 25.0f);
    //     if (ImGui::Button("...", ImVec2{ 20.0f, 20.0f }))
    //         ImGui::OpenPopup("ComponentSettings");
    //     ImGui::PopStyleVar();
        
    //     bool remove_component = false;
    //     if (ImGui::BeginPopup("ComponentSettings")) {
    //         if (ImGui::MenuItem("Remove component")) {
    //             remove_component = true;
    //         }
    //         ImGui::EndPopup();
    //     }

    //     if (open) {
    //         auto& tc = entity.GetComponent<TransformComponent>();

    //         auto rotation = glm::degrees(tc.Rotation);
    //         ImGui::DragFloat3("Translation", glm::value_ptr(tc.Translation), 0.1f);
    //         ImGui::DragFloat3("Rotation",    glm::value_ptr(rotation),       1.0f);
    //         ImGui::DragFloat3("Scale",       glm::value_ptr(tc.Scale),       0.1f);
    //         tc.Rotation = glm::radians(rotation);

    //         ImGui::TreePop();
    //     }

    //     if (remove_component)
    //         entity.RemoveComponent<TransformComponent>();
    // }
// }

/*

void SceneHierarchyPanel::DrawComponents(Entity entity) {
    // TODO:add function for drawing components
    // TODO:template<typename T, Func> DrawComponent<T>(std::string name, Func func);

    if (entity.HasComponent<TagComponent>()) {
        auto& tag = entity.GetComponent<TagComponent>().Tag;

        static char buffer[256];
        memset(buffer, 0, sizeof(buffer));
        strcpy_s(buffer, sizeof(buffer), tag.c_str());

        if (ImGui::InputText("Tag", buffer, sizeof(buffer))) {
            tag = std::string(buffer);
        }
    }

    const ImGuiTreeNodeFlags tree_node_flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap;

    if (entity.HasComponent<TransformComponent>()) {
        bool open = ImGui::TreeNodeEx((void*)typeid(TransformComponent).hash_code(), tree_node_flags, "%s", "Transform");

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4.0f, 4.0f });
        ImGui::SameLine(ImGui::GetWindowWidth() - 25.0f);
        if (ImGui::Button("...", ImVec2{ 20.0f, 20.0f }))
            ImGui::OpenPopup("ComponentSettings");
        ImGui::PopStyleVar();
        
        bool remove_component = false;
        if (ImGui::BeginPopup("ComponentSettings")) {
            if (ImGui::MenuItem("Remove component")) {
                remove_component = true;
            }
            ImGui::EndPopup();
        }

        if (open) {
            auto& tc = entity.GetComponent<TransformComponent>();

            auto rotation = glm::degrees(tc.Rotation);
            ImGui::DragFloat3("Translation", glm::value_ptr(tc.Translation), 0.1f);
            ImGui::DragFloat3("Rotation",    glm::value_ptr(rotation),       1.0f);
            ImGui::DragFloat3("Scale",       glm::value_ptr(tc.Scale),       0.1f);
            tc.Rotation = glm::radians(rotation);

            ImGui::TreePop();
        }

        if (remove_component)
            entity.RemoveComponent<TransformComponent>();
    }

    if (entity.HasComponent<CameraComponent>()) {
        if (ImGui::TreeNodeEx((void*)typeid(CameraComponent).hash_code(), tree_node_flags, "%s", "Camera")) {
            auto& cc = entity.GetComponent<CameraComponent>();

            ImGui::Checkbox("Primary camera", &cc.Primary);

            static const char* projection_type_string[] = { "Perspective", "Orthographic" };
            static const char* current_projection_type_string = projection_type_string[(int)cc.Camera.GetProjectionType()];
            if (ImGui::BeginCombo("Projection", current_projection_type_string)) {
                
                for (int i = 0; i < 2; ++i) {
                    bool is_selected = current_projection_type_string == projection_type_string[i];
                    if (ImGui::Selectable(projection_type_string[i], is_selected)) {
                        current_projection_type_string = projection_type_string[i];
                        cc.Camera.SetProjectionType((SceneCamera::ProjectionType)i);
                    }

                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }

            if (cc.Camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective) {
                float fov       = cc.Camera.GetPerspectiveFOV();
                float near_clip = cc.Camera.GetPerspectiveNearClip();
                float far_clip  = cc.Camera.GetPerspectiveFarClip();

                if (ImGui::DragFloat("Focal length", &fov, 0.1f, 1.0f, 250.0f))
                    cc.Camera.SetPerspectiveFOV(fov);

                if (ImGui::DragFloat("Near clip", &near_clip, 0.1f, 0.0001f, 10000.0f))
                    cc.Camera.SetPerspectiveNearClip(near_clip);

                if (ImGui::DragFloat("Far clip", &far_clip, 0.1f, 0.0001f, 10000.0f))
                    cc.Camera.SetPerspectiveFarClip(far_clip);
            }

            if (cc.Camera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic) {

            }

            ImGui::TreePop();
        }
    }

    if (entity.HasComponent<MeshRendererComponent>()) {
        if (ImGui::TreeNodeEx((void*)typeid(MeshRendererComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "%s", "Mesh Renderer")) {
            auto& tc = entity.GetComponent<MeshRendererComponent>();

            // ImGui::TextUnformatted("Mesh albedo");
            // ImVec2 image_size = { 256, 256 };
            // auto albedo_ID = tc.Material->GetAlbedo()->GetRendererID();
            // ImGui::Image(reinterpret_cast<void*>(albedo_ID), image_size, ImVec2(0, 1), ImVec2(1, 0));

            ImGui::TreePop();
        }
    }

    if (entity.HasComponent<PointLightComponent>()) {
        if (ImGui::TreeNodeEx((void*)typeid(PointLightComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "%s", "Point Light Component")) {
            auto& tc = entity.GetComponent<PointLightComponent>();

            ImGui::ColorEdit3("Color", glm::value_ptr(tc.Color));
            ImGui::DragFloat("Radiant Flux", &tc.RadiantFlux);

            ImGui::TreePop();
        }
    }

    if (entity.HasComponent<DirectionalLightComponent>()) {
        if (ImGui::TreeNodeEx((void*)typeid(DirectionalLightComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "%s", "Directional Light Component")) {
            auto& tc = entity.GetComponent<DirectionalLightComponent>();

            glm::vec2 yaw_pitch = { glm::degrees(tc.Yaw), glm::degrees(tc.Pitch) };
            ImGui::ColorEdit3("Color",  glm::value_ptr(tc.Color));
            ImGui::DragFloat2("Direction", glm::value_ptr(yaw_pitch), 1.0f);
            ImGui::DragFloat("Radiant Intensity", &tc.RadiantIntensity);
            tc.Yaw   = glm::radians(yaw_pitch[0]);
            tc.Pitch = glm::radians(yaw_pitch[1]);

            ImGui::TreePop();
        }
    }
}

*/

void PropertiesWindow::Tag(Entity& entity) {
    auto& tag = entity.GetComponent<TagComponent>().Tag;

    static char buffer[256];
    memset(buffer, 0, sizeof(buffer));
    strcpy_s(buffer, sizeof(buffer), tag.c_str());

    if (ImGui::InputText("Tag", buffer, sizeof(buffer))) {
        tag = std::string(buffer);
    }
}

template<typename Component, typename Func>
void PropertiesWindow::PropertyNode(Entity& entity, Component& component, std::string node_name, bool removable, Func widget_func) {
    const ImGuiTreeNodeFlags tree_node_flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap;

    bool open = ImGui::TreeNodeEx((void*)typeid(component).hash_code(), tree_node_flags, "%s", node_name.c_str());

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4.0f, 4.0f });
    ImGui::SameLine(ImGui::GetWindowWidth() - 25.0f);
    if (ImGui::Button("...", ImVec2{ 20.0f, 20.0f }))
        ImGui::OpenPopup("ComponentSettings");
    ImGui::PopStyleVar();
    
    bool remove_component = false;
    if (ImGui::BeginPopup("ComponentSettings")) {
        if (removable) {
            if (ImGui::MenuItem("Remove component"))
                remove_component = true;
        }
        ImGui::EndPopup();
    }

    if (open) {
        widget_func();
        ImGui::TreePop();
    }

    if (removable && remove_component)
        entity.RemoveComponent<Component>();
}

template<typename Component>
void PropertiesWindow::Property(Entity& entity) { }

template<> void PropertiesWindow::Property<ColorComponent>(Entity& entity) {
    auto& cc = entity.GetComponent<ColorComponent>();
    
    PropertyNode(entity, cc, "Color", false, [&entity, &cc](){
        ImVec4 color = rlImGuiColors::Convert(cc.Color);

        if (ImGui::ColorEdit4("ColorEdit##1", (float*)&color)) {
            cc.Color = rlImGuiColors::Convert(color);
        }
    });
}

template<> void PropertiesWindow::Property<LineComponent>(Entity& entity) {
    auto& lc = entity.GetComponent<LineComponent>();
    PropertyNode(entity, lc, "Line", true, [&entity, &lc](){
        
        float begin[2] = { lc.Begin.x, lc.Begin.y };
        float end[2]   = { lc.End.x,   lc.End.y   };

        ImGui::DragFloat("Thickness", &lc.Thickness);

        if (ImGui::DragFloat2("Begin", begin)) {
            lc.Begin.x = begin[0];
            lc.Begin.y = begin[1];
        }
        if (ImGui::DragFloat2("End", end)) {
            lc.End.x = end[0];
            lc.End.y = end[1];
        }
    });
}

template<> void PropertiesWindow::Property<RectangleComponent>(Entity& entity) {
    auto& rc = entity.GetComponent<RectangleComponent>();
    PropertyNode(entity, rc, "Rectangle", true, [&entity, &rc](){
        
        float begin[2] = { rc.Begin.x, rc.Begin.y };
        float end[2]   = { rc.End.x,   rc.End.y   };

        if (ImGui::DragFloat2("Begin", begin)) {
            rc.Begin.x = begin[0];
            rc.Begin.y = begin[1];
        }
        if (ImGui::DragFloat2("End", end)) {
            rc.End.x = end[0];
            rc.End.y = end[1];
        }
    });
}

template<> void PropertiesWindow::Property<EllipseComponent>(Entity& entity) {
    auto& rc = entity.GetComponent<EllipseComponent>();
    PropertyNode(entity, rc, "Rectangle", true, [&entity, &rc](){
        
        float center[2] = { rc.Center.x, rc.Center.y };

        if (ImGui::DragFloat2("Center", center)) {
            rc.Center.x = center[0];
            rc.Center.y = center[1];
        }

        ImGui::DragFloat("Radius y", &rc.RadiusX);
        ImGui::DragFloat("Radius x", &rc.RadiusY);
    });
}

}
