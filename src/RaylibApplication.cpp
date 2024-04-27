#include "RaylibApplication.hpp"

#include <raylib.h>

namespace cw {

RaylibApplication* RaylibApplication::m_Instance = nullptr;

RaylibApplication::RaylibApplication() {
    Init();
}

RaylibApplication::~RaylibApplication() {
    Destroy();
}

void RaylibApplication::Init() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT | FLAG_MSAA_4X_HINT);

    InitWindow(m_Width, m_Height, "Honey-Honey-Honey-Ho");

    SetTargetFPS(144);

    m_Environment = CreateRef<Environment>();

    m_Instance = this;
}

void RaylibApplication::Destroy() {
    CloseWindow();
}

void RaylibApplication::Run() {
    OnSetup();
    while(m_Running) {
        OnUpdate();
        OnRender();
    }
    OnShutdown();
};

void RaylibApplication::OnSetup() {
    m_Environment->OnSetup();
}

void RaylibApplication::OnUpdate() {
    HandleScreenResize();

    if (WindowShouldClose())
        m_Running = false;

    m_Environment->OnUpdate();
};

void RaylibApplication::OnRender() {
    m_Environment->OnRender();
};

void RaylibApplication::OnShutdown() {
    m_Environment->OnShutdown();
}

void RaylibApplication::HandleScreenResize() {
    if (IsWindowResized() && !IsWindowFullscreen()) {
        m_Width = GetScreenWidth();
        m_Height = GetScreenHeight();
    }
}

}