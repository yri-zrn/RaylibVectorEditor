#pragma once

#include "Core.hpp"
#include "Components.hpp"

#include <entt.hpp>
#include <list>
#include <string>

namespace cw {

class Archive;

class SceneSerializer;
class ViewportWindow;
class Entity;

class Scene {
private:
    friend Entity;
    friend SceneSerializer;

public:
    Scene() = default;
    ~Scene() = default;

    void SetViewportContext(ViewportWindow* context) { m_ViewportContext = context; }

    void Clear() { m_Registry.clear(); }

    Entity CreateEntity(const std::string& name = "Unnamed");
    Entity CreateEntity(uint32_t handle, const std::string& name = "Unnamed");
    void DestroyEntity(Entity& entity);

    void DeselectEntity(Entity entity);
    void DeselectEntities();
    void SelectEntity(Entity entity);
    bool HasSelectedEntities() const;
    Entity GetSelectedEntity();
    std::vector<Entity> GetSelectedEntities();

    std::vector<Entity> GetEntityStack();
    void PushEntityBack(const Entity& entity);
    void PushEntityFront(const Entity& entity);

    template<typename Component>
    int GetCount() const { return m_Registry.view<Component>().size(); }

    int GetLineCount()      const { return GetCount<LineComponent>();      }
    int GetRectangleCount() const { return GetCount<RectangleComponent>(); }
    int GetEllipseCount()   const { return GetCount<EllipseComponent>();   }

    template<typename Func>
    void ForEach(Func func) { m_Registry.view<entt::entity>().each(func); }

    auto begin() { return m_Registry.view<entt::entity>().begin(); }
    auto end()   { return m_Registry.view<entt::entity>().end();   }

private:
    template<typename T>
    void OnComponentAdded(Entity& entity, T& component);

private:
    entt::registry m_Registry;
    std::vector<entt::entity> m_EntityStack;

    ViewportWindow* m_ViewportContext;
};

}
