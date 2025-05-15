#pragma once
#include <memory>
namespace  qyhs
{
    class WindowSystem;
    class RenderSystem;
    struct WindowUIInitInfo
    {
        std::shared_ptr<WindowSystem> window;
        std::shared_ptr<RenderSystem> render_system;
    };

    class WindowUI
    {
    public:
        virtual void initialize(WindowUIInitInfo * info);
        virtual void preRender();
    };
    
}
