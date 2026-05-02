#pragma once

#include "Event.h"
#include "../inputs/KeyCodes.h"
#include "../time/Time.h"

namespace Engine
{
    class MouseButtonPressedEvent : public EventBase<MouseButtonPressedEvent>
    {
    public:
        MouseButtonPressedEvent(MouseButton btn) : button(btn), tick(Time::GetTicks()) {}

        MouseButton GetButton() const { return button; }
        int GetTick() const { return tick; }

        const char* GetName() const override { return "MouseButtonPressedEvent"; }

    private:
        MouseButton button;
        int tick;
    };

    class MouseButtonReleasedEvent : public EventBase<MouseButtonReleasedEvent>
    {
    public:
        MouseButtonReleasedEvent(MouseButton btn) : button(btn), tick(Time::GetTicks()) {}

        MouseButton GetButton() const { return button; }
        int GetTick() const { return tick; }

        const char* GetName() const override { return "MouseButtonReleasedEvent"; }

    private:
        MouseButton button;
        int tick;
    };

    class MouseMovedEvent : public EventBase<MouseMovedEvent>
    {
    public:
        /// Constructor.
        /// @note Ensure you pass logical coordinates scaled from the window, not raw screen pixels!
        MouseMovedEvent(float xPos, float yPos)
            : x(xPos), y(yPos), tick(Time::GetTicks())
        {
        }

        float GetX() const { return x; }
        float GetY() const { return y; }
        int GetTick() const { return tick; }

        const char* GetName() const override { return "MouseMovedEvent"; }

    private:
        float x, y; // Logical/Virtual position
        int tick;
    };

    class KeyPressedEvent : public EventBase<KeyPressedEvent>
    {
    public:
        KeyPressedEvent(Key k) : key(k), tick(Time::GetTicks()) {}

        Key GetKey() const { return key; }
        int GetTick() const { return tick; }

        const char* GetName() const override { return "KeyPressedEvent"; }

    private:
        Key key;
        int tick;
    };

    class KeyReleasedEvent : public EventBase<KeyReleasedEvent>
    {
    public:
        KeyReleasedEvent(Key k) : key(k), tick(Time::GetTicks()) {}

        Key GetKey() const { return key; }
        int GetTick() const { return tick; }

        const char* GetName() const override { return "KeyReleasedEvent"; }

    private:
        Key key;
        int tick;
    };

    /// Action categories for input logging and filtering.
    enum class ActionCategory : uint8_t
    {
        Gameplay = 0, // Recorded in normal replays
        System = 1    // Recorded only in QA/Debug modes (Pause, Menus, etc.)
    };

    struct ActionState
    {
        float value = 0.0f;
        bool isPressed = false;
        bool wasPressed = false;
        ActionCategory category = ActionCategory::Gameplay;
    };

    class ActionChangedEvent : public EventBase<ActionChangedEvent>
    {
    public:
        ActionChangedEvent(uint32_t hash, float val, ActionCategory cat)
            : actionHash(hash), value(val), category(cat), tick(Time::GetTicks())
        {
        }

        uint32_t GetHash() const { return actionHash; }
        float GetValue() const { return value; }
        int GetTick() const { return tick; }
        ActionCategory GetCategory() const { return category; }

        const char* GetName() const override { return "ActionChangedEvent"; }

    private:
        uint32_t actionHash;
        float value;
        int tick;
        ActionCategory category;
    };

    class MouseScrolledEvent : public EventBase<MouseScrolledEvent>
    {
    public:
        MouseScrolledEvent(float absScroll, float d)
            : absoluteScroll(absScroll), delta(d), tick(Time::GetTicks())
        {
        }

        float GetAbsolute() const { return absoluteScroll; }
        float GetDelta() const { return delta; }
        int GetTick() const { return tick; }

        const char* GetName() const override { return "MouseScrolledEvent"; }

    private:
        float absoluteScroll;
        float delta;
        int tick;
    };
}