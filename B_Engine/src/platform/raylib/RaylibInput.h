#pragma once

#include <optional>
#include "../../core/interfaces/IInput.h"

namespace Engine
{
    /// Raylib implementation of the IInput interface.
    class RaylibInput : public IInput
    {
    public:
        RaylibInput() = default;
        ~RaylibInput() override = default;

        bool IsKeyDown(Key key) const override;
        bool IsKeyPressed(Key key) const override;
        bool IsKeyReleased(Key key) const override;

        bool IsMouseButtonDown(MouseButton button) const override;
        bool IsMouseButtonPressed(MouseButton button) const override;
        bool IsMouseButtonReleased(MouseButton button) const override;

        Vector2f GetMousePosition() const override;
        std::optional<Vector2f> GetMouseDelta() const override;
        float GetMouseWheel() override;
    };
}