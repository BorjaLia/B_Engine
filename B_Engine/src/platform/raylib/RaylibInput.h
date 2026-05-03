#pragma once

#include <optional>
#include "../../inputs/InputBase.h"

namespace Engine
{
    /// Raylib implementation of the InputBase class.
    class RaylibInput : public InputBase
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