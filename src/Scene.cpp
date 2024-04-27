#include "Scene.hpp"
#include "Entity.hpp"
#include "ViewportWindow.hpp"

#include <iostream>
#include <algorithm>

namespace cw {

Entity Scene::CreateEntity(const std::string& name) {
    auto entity_handle = m_Registry.create();
    Entity entity = { entity_handle, this };
    m_EntityStack.push_back(entity_handle);

    entity.AddComponent<TagComponent>(name);
    entity.AddComponent<ColorComponent>();
    return entity;
}

Entity Scene::CreateEntity(uint32_t handle, const std::string& name) {
    Entity entity = { m_Registry.create((entt::entity)handle), this };
    m_EntityStack.push_back((entt::entity)handle);

    entity.AddComponent<TagComponent>(name);
    entity.AddComponent<ColorComponent>();
    return entity;
}

void Scene::DestroyEntity(Entity& entity) {
    m_Registry.destroy(entity);
    m_EntityStack.erase(std::find(m_EntityStack.begin(), m_EntityStack.end(), (entt::entity)entity));
}

void Scene::SelectEntity(Entity entity)   { m_Registry.emplace<SelectedComponent>((entt::entity)entity);   }
void Scene::DeselectEntities()            { m_Registry.clear<SelectedComponent>();                         }
void Scene::DeselectEntity(Entity entity) { m_Registry.remove<SelectedComponent>((entt::entity)entity);    }
bool Scene::HasSelectedEntities() const   { return !m_Registry.view<SelectedComponent>().empty();          }
Entity Scene::GetSelectedEntity()         { return { m_Registry.view<SelectedComponent>().front(), this }; }

std::vector<Entity> Scene::GetSelectedEntities() {
    auto view = m_Registry.view<SelectedComponent>();
    std::vector<Entity> output;
    output.reserve(view.size());
    std::transform(view.begin(), view.end(), std::back_inserter(output), [this](entt::entity entity_handle) {
        return Entity{ entity_handle, this };
    });
    return output;
}

std::vector<Entity> Scene::GetEntityStack() {
    std::vector<Entity> stack;
    stack.reserve(m_EntityStack.size());
    std::transform(m_EntityStack.begin(), m_EntityStack.end(), std::back_inserter(stack), [this](entt::entity entity_handle) {
        return Entity{ entity_handle, this };
    });
    return stack;
}

void Scene::PushEntityBack(const Entity& entity) {
    auto pos = std::find(m_EntityStack.begin(), m_EntityStack.end(), (entt::entity)entity);
    if (pos != std::prev(m_EntityStack.end()))
        std::swap(*pos, *std::next(pos));
}

void Scene::PushEntityFront(const Entity& entity) {
    auto pos = std::find(m_EntityStack.begin(), m_EntityStack.end(), (entt::entity)entity);
    if (pos != m_EntityStack.begin())
        std::swap(*pos, *std::prev(pos));
}

#ifdef DEPRECATED_COMPONENT_ADDED
template<typename T>
void Scene::OnComponentAdded(Entity& entity, T& component) { }

template<>
void Scene::OnComponentAdded(Entity& entity, TagComponent& component) { }

template<>
void Scene::OnComponentAdded(Entity& entity, ColorComponent& component) { }

template<>
void Scene::OnComponentAdded(Entity& entity, LineComponent& component) { m_EntityStack.push_back(entity); }

template<>
void Scene::OnComponentAdded(Entity& entity, RectangleComponent& component) { m_EntityStack.push_back(entity); }

template<>
void Scene::OnComponentAdded(Entity& entity, EllipseComponent& component) { m_EntityStack.push_back(entity); }
#endif

}
