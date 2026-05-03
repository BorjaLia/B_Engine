#pragma once

#include <optional>
#include "KeyCodes.h"
#include "../utils/Math.h"

namespace Engine
{
    class EventBus;

    /// Base class for hardware input handling and event polling.
    /// @ingroup Core
    class InputBase
    {
    public:
        virtual ~InputBase() = default;

        /// Reads hardware state and pushes events to the EventBus.
        virtual void Update(EventBus& eventBus);

        virtual bool IsKeyDown(Key key) const = 0;
        virtual bool IsKeyPressed(Key key) const = 0;
        virtual bool IsKeyReleased(Key key) const = 0;

        virtual bool IsMouseButtonDown(MouseButton button) const = 0;
        virtual bool IsMouseButtonPressed(MouseButton button) const = 0;
        virtual bool IsMouseButtonReleased(MouseButton button) const = 0;

        virtual Vector2f GetMousePosition() const = 0;
        virtual std::optional<Vector2f> GetMouseDelta() const = 0;
        virtual float GetMouseWheel() = 0;

    protected:
        bool previousMouseStates[static_cast<int>(MouseButton::Count)] = { false };
        bool previousKeyStates[static_cast<int>(Key::Count)] = { false };
        Vector2f previousMousePos;
        float accumulatedScroll = 0.0f;
    };
}