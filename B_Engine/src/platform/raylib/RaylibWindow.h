#pragma once

#include <memory>
#include <string>

#include "../../core/interfaces/IWindow.h"
#include "../../math/Vector2.h"

namespace Engine
{
    /// Raylib implementation of the IWindow interface.
    /// Manages OS window creation, events, and initializes Raylib subsystems.
    class RaylibWindow : public IWindow
    {
    public:
        RaylibWindow();
        ~RaylibWindow() override;

        bool Initialize(const Vector2i& size, const std::string& title) override;
        void Shutdown() override;

        void OnUpdate() override;

        inline Vector2i GetSize() const override { return data.size; }
        bool WindowShouldClose() const override;

        void SetTitle(const std::string& title) override;
        void SetSize(const Vector2i& size) override;
        void SetTargetFPS(int fps) override;
        void SetFullscreen(bool fullscreen) override;
        bool IsFullscreen() const override;
        void SetVSync(bool vsync) override;
        void SetHideCursor(bool show) override;

        inline void SetEventCallback(const EventCallbackFn& callback) override { eventCallback = callback; }

        inline RendererBase* GetRenderer() const override { return renderer.get(); }
        inline InputBase* GetInput() const override { return input.get(); }

    private:
        std::unique_ptr<RendererBase> renderer;
        std::unique_ptr<InputBase> input;
        EventCallbackFn eventCallback;

        struct WindowData
        {
            std::string title;
            Vector2i size;
        } data;
    };
}