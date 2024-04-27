#pragma once

#include "Entity.hpp"

namespace cw {

class ViewportWindow;

class ViewportCollisionResolver {
public:
    void SetContext(ViewportWindow* context) { m_ViewportContext = context; }

    Entity CheckCollision(const Vector2& position) const;
    std::vector<Entity> CheckCollision(const Rectangle& rec) const;

private:
    ViewportWindow* m_ViewportContext;
};

}