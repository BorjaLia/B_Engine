#pragma once

#include <windows.h>
#include <optional>
#include "../../inputs/InputBase.h"

namespace Engine
{
    /// Windows Console implementation of the InputsBase class.
    /// Handles Win32 API calls for keyboard and mouse capture.
    class ConsoleInput : public InputBase
    {
    public:
        ConsoleInput();
        ~ConsoleInput() override = default;

        /// Sets the virtual screen size to map raw mouse pixels to engine coordinates.
        void SetVirtualSize(const Vector2i& vSize) { virtualSize = vSize; }

        bool IsKeyDown(Key key) const override;
        bool IsKeyPressed(Key key) const override;
        bool IsKeyReleased(Key key) const override;

        bool IsMouseButtonDown(MouseButton button) const override;
        bool IsMouseButtonPressed(MouseButton button) const override;
        bool IsMouseButtonReleased(MouseButton button) const override;

        Vector2f GetMousePosition() const override;
        std::optional<Vector2f> GetMouseDelta() const override;
        float GetMouseWheel() override;

    private:
        HWND consoleWindow;
        Vector2i virtualSize;
    };
}