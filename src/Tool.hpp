#pragma once

#include "RaylibObject.hpp"

namespace cw {

class ToolManager;
class Scene;
class ViewportWindow;

class Tool : public RaylibObject {
public:
    enum class PrimitiveType {
        None = 0,
        Line,
        Rectangle,
        Ellipse
    };

private:
    friend ToolManager;

public:
    virtual ~Tool() = default;
};

class CreationTool : public Tool {
public:
    struct Context {
        Scene* Scene;
        ViewportWindow* Viewport;
    };
public:
    CreationTool() = default;
    ~CreationTool() = default;

    void SetContext(const Context& context) { m_Context = context; }
    void SetActive(bool active = true) { m_Active = active; }
    void SetPrimitiveColor(const Color& color) { m_PrimitiveColor = color; }
    void SetPrimitiveType(PrimitiveType type) { m_PrimitiveType = type; }

    bool IsActive() const { return m_Active; }
    bool IsDrawing() const { return m_Drawing; }
    const Vector2& GetInitialPosition() const { return m_InitialPosition; }
    const Vector2& GetCurrentPosition() const { return m_CurrentPosition; }
    PrimitiveType GetPrimitiveType() const { return m_PrimitiveType; }

    virtual void OnUpdate() override;
    virtual void OnRender() override;

private:
    void CreateEntity();

private:
    bool m_Active   = true;
    bool m_Drawing  = false;
    Vector2 m_InitialPosition;
    Vector2 m_CurrentPosition;

    PrimitiveType m_PrimitiveType;
    Color m_PrimitiveColor = { 60, 52, 224, 255 };

    Context m_Context;
};

class SelectionTool : public Tool {
public:
    struct Context {
        Scene* Scene;
        ViewportWindow* Viewport;
    };
public:
    SelectionTool() = default;
    ~SelectionTool() = default;

    void SetContext(const Context& context) { m_Context = context; }
    void SetActive(bool active = true) { m_Active = active; }
    void SetPrimitiveType(PrimitiveType type) { m_PrimitiveType = type; }

    bool IsActive() const { return m_Active; }
    bool IsSelectingRegion() const { return m_SelectingRegion; }
    const Vector2& GetInitialPosition() const { return m_InitialPosition; }
    const Vector2& GetCurrentPosition() const { return m_CurrentPosition; }
    PrimitiveType GetPrimitiveType() const { return m_PrimitiveType; }

    virtual void OnUpdate() override;
    virtual void OnRender() override;

private:
    void SelectEntity(const Vector2& position);
    void SelectRegion(const Rectangle& rec);
    void DrawSelection();
    void ShowSelectedRegion();

private:
    bool m_Active           = true;
    bool m_StartedSelecting = false;
    bool m_SelectingRegion  = false;
    Vector2 m_InitialPosition;
    Vector2 m_CurrentPosition;

    PrimitiveType m_PrimitiveType;
    Context m_Context;
};

class Gizmo {
public:
    struct Context {
        Scene* Scene;
        ViewportWindow* Viewport;
    };
};

class ToolManager : public RaylibObject {
public:
    ~ToolManager() = default;

private:

};

}