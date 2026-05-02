#pragma once

#include <memory>
#include <string>
#include <windows.h>

#include "../../core/interfaces/IWindow.h"

namespace Engine
{
    /// Windows Console implementation of the IWindow interface.
    /// Manages the OS window lifecycle and initializes console input and rendering.
    class ConsoleWindow : public IWindow
    {
    public:
        ConsoleWindow() = default;
        ~ConsoleWindow() override;

        bool Initialize(const Vector2i& size, const std::string& title) override;
        void Shutdown() override;

        void OnUpdate() override;

        Vector2i GetSize() const override { return windowSize; }
        bool WindowShouldClose() const override;

        void SetTitle(const std::string& title) override;
        void SetSize(const Vector2i& /* size */) override {}
        void SetTargetFPS(int /*fps*/) override {}
        void SetFullscreen(bool /*fullscreen*/) override {}
        bool IsFullscreen() const override { return false; }
        void SetVSync(bool /*vsync*/) override {}
        void SetHideCursor(bool show) override;

        void SetEventCallback(const EventCallbackFn& callback) override { eventCallback = callback; }

        IRenderer* GetRenderer() const override { return renderer.get(); }
        IInput* GetInput() const override { return input.get(); }

    private:
        std::unique_ptr<IRenderer> renderer;
        std::unique_ptr<IInput> input;
        EventCallbackFn eventCallback;

        Vector2i windowSize;
        std::string windowTitle;
    };
}