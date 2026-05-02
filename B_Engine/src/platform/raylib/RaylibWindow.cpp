#include "RaylibWindow.h"

#include <raylib.h> // Unique access to window API
#include "RaylibRenderer.h"
#include "RaylibInput.h"
#include "../../events/ApplicationEvents.h"

namespace Engine
{
    RaylibWindow::RaylibWindow()
    {
        data.size = Vector2i(0, 0);
        data.title = "";
    }

    RaylibWindow::~RaylibWindow()
    {
        Shutdown();
    }

    bool RaylibWindow::Initialize(const Vector2i& size, const std::string& title)
    {
        // Set the minimum log level to display from Raylib internals.
        // LOG_NONE will hide all native warnings to keep our engine console clean.
        ::SetTraceLogLevel(LOG_NONE);

        data.size = size;
        data.title = title;

        // 1. Request window from OS
        ::InitWindow(size.x, size.y, title.c_str());
        ::SetExitKey(KEY_NULL);

        if (!::IsWindowReady())
        {
            return false;
        }

        // 2. Initialize graphics and input handlers
        renderer = std::make_unique<RaylibRenderer>();
        input = std::make_unique<RaylibInput>();

        ::SetWindowState(FLAG_WINDOW_RESIZABLE);

        if (!renderer->Initialize())
        {
            return false;
        }

        return true;
    }

    void RaylibWindow::Shutdown()
    {
        if (::IsWindowReady())
        {
            if (renderer) renderer->Shutdown();
        }
    }

    void RaylibWindow::OnUpdate()
    {
        if (WindowShouldClose())
        {
            if (eventCallback)
            {
                WindowCloseEvent event;
                eventCallback(event);
            }
        }

        if (IsWindowResized())
        {
            data.size.x = ::GetScreenWidth();
            data.size.y = ::GetScreenHeight();

            if (eventCallback)
            {
                WindowResizeEvent event(data.size.x, data.size.y);
                eventCallback(event);
            }
        }
    }

    bool RaylibWindow::WindowShouldClose() const
    {
        return ::WindowShouldClose();
    }

    void RaylibWindow::SetTitle(const std::string& title)
    {
        ::SetWindowTitle(std::string(title).c_str());
    }

    void RaylibWindow::SetSize(const Vector2i& size)
    {
        ::SetWindowSize(size.x, size.y);
        data.size = size;
    }

    void RaylibWindow::SetTargetFPS(int fps)
    {
        ::SetTargetFPS(fps);
    }

    void RaylibWindow::SetFullscreen(bool fullscreen)
    {
        if (fullscreen != ::IsWindowFullscreen())
        {
            ::ToggleFullscreen();
        }
    }

    bool RaylibWindow::IsFullscreen() const
    {
        return ::IsWindowFullscreen();
    }

    void RaylibWindow::SetVSync(bool vsync)
    {
        if (vsync)
        {
            ::SetWindowState(FLAG_VSYNC_HINT);
        }
        else
        {
            ::ClearWindowState(FLAG_VSYNC_HINT);
        }
    }

    void RaylibWindow::SetHideCursor(bool show)
    {
        if (show)
        {
            ::ShowCursor();
        }
        else
        {
            ::HideCursor();
        }
    }
}