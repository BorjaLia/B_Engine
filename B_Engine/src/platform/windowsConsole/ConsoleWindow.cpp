#include "ConsoleWindow.h"

#include "ConsoleRenderer.h"
#include "ConsoleInput.h"

namespace Engine
{
    ConsoleWindow::~ConsoleWindow() { Shutdown(); }

    bool ConsoleWindow::Initialize(const Vector2i& size, const std::string& title)
    {
        windowSize = size;
        windowTitle = title;
        SetTitle(title);

        // Hide the native Windows text cursor to avoid visual artifacts
        SetHideCursor(true);

        auto consoleRenderer = std::make_unique<ConsoleRenderer>();
        consoleRenderer->SetVirtualSize(size);
        renderer = std::move(consoleRenderer);

        auto consoleInput = std::make_unique<ConsoleInput>();
        consoleInput->SetVirtualSize(size);
        input = std::move(consoleInput);

        return renderer->Initialize();
    }

    void ConsoleWindow::Shutdown()
    {
        if (renderer) renderer->Shutdown();
    }

    void ConsoleWindow::OnUpdate()
    {
        // In a pure console window, we would listen to OS close events (Ctrl+C).
        // For now, this is kept simple.
    }

    bool ConsoleWindow::WindowShouldClose() const
    {
        // This is typically overridden or checked by an ESC key binding in the InputManager
        return false;
    }

    void ConsoleWindow::SetTitle(const std::string& title)
    {
        SetConsoleTitleA(title.c_str());
    }

    void ConsoleWindow::SetHideCursor(bool show)
    {
        HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_CURSOR_INFO info;
        info.dwSize = 100;
        info.bVisible = show ? TRUE : FALSE;
        SetConsoleCursorInfo(consoleHandle, &info);
    }
}