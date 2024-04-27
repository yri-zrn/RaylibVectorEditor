#pragma once

#include "RaylibObject.hpp"
#include "Environment.hpp"

namespace cw {

class RaylibApplication : public RaylibObject {
public:
    RaylibApplication();
    virtual ~RaylibApplication();

    static RaylibApplication* Get() { return m_Instance; }

    void Run();
    void Close() { m_Running = false; }

    virtual void OnSetup() override;
    virtual void OnUpdate() override;
    virtual void OnRender() override;
    virtual void OnShutdown() override;

private:
    void Init();
    void Destroy();

private:
    void HandleScreenResize();

private:
    int m_Width  = 1600;
    int m_Height = 900;
    bool m_Running = true;
    static RaylibApplication* m_Instance;
    // test
    // static Application& m_Instance;
    
    Ref<Environment> m_Environment;
};


};