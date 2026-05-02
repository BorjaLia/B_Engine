#pragma once

#include <memory>
#include <string>
#include <functional>

#include "../../events/Event.h"
#include "IRenderer.h"
#include "IInput.h"
#include "../../utils/Math.h"

namespace Engine
{
    /// Interface for the operating system window manager.
    /// @ingroup Core
    class IWindow
    {
    public:
        using EventCallbackFn = std::function<void(Event&)>;

        virtual ~IWindow() = default;

        virtual bool Initialize(const Vector2i& size, const std::string& title) = 0;
        virtual void Shutdown() = 0;

        /// Core polling loop. Expected to be called every frame.
        virtual void OnUpdate() = 0;

        virtual Vector2i GetSize() const = 0;
        virtual bool WindowShouldClose() const = 0;

        virtual void SetTitle(const std::string& title) = 0;
        virtual void SetSize(const Vector2i& size) = 0;
        virtual void SetTargetFPS(int fps) = 0;
        virtual void SetFullscreen(bool fullscreen) = 0;
        virtual bool IsFullscreen() const = 0;
        virtual void SetVSync(bool vsync) = 0;
        virtual void SetHideCursor(bool show) = 0;

        /// Injects the callback function used to pipe window events to the EventBus.
        virtual void SetEventCallback(const EventCallbackFn& callback) = 0;

        virtual IRenderer* GetRenderer() const = 0;
        virtual IInput* GetInput() const = 0;
    };
}