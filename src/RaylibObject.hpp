#pragma once
#include <raylib.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <rlImGui.h>
#include <rlgl.h>
#include <raymath.h>
#include <rlImGuiColors.h>

namespace cw {

class RaylibObject {
public:
    virtual ~RaylibObject() = default;

    virtual void OnSetup() { }
    virtual void OnUpdate() { }
    virtual void OnRender() { }
    virtual void OnShutdown() { }
};

}