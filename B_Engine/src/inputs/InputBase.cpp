#include "InputBase.h"

#include <cmath>

#include "../core/Application.h"
#include "../events/EventBus.h"
#include "../events/InputEvents.h"

namespace Engine
{
    void InputBase::Update(EventBus& eventBus)
    {
        // 1. --- KEYBOARD ---
        for (int i = 1; i < static_cast<int>(Key::Count); ++i)
        {
            Key currentKey = static_cast<Key>(i);
            bool isDown = IsKeyDown(currentKey);
            bool wasDown = previousKeyStates[i];

            if (isDown && !wasDown)
            {
                KeyPressedEvent e(currentKey);
                eventBus.Publish(e);
            }
            else if (!isDown && wasDown)
            {
                KeyReleasedEvent e(currentKey);
                eventBus.Publish(e);
            }

            previousKeyStates[i] = isDown;
        }

        // 2. --- MOUSE BUTTONS ---
        for (int i = 0; i < 5; ++i)
        {
            MouseButton currentBtn = static_cast<MouseButton>(i);
            bool isDown = IsMouseButtonDown(currentBtn);
            bool wasDown = previousMouseStates[i];

            if (isDown && !wasDown)
            {
                MouseButtonPressedEvent e(currentBtn);
                eventBus.Publish(e);
            }
            else if (!isDown && wasDown)
            {
                MouseButtonReleasedEvent e(currentBtn);
                eventBus.Publish(e);
            }

            previousMouseStates[i] = isDown;
        }

        // 3. --- MOUSE MOVEMENT ---
        Vector2f rawMouse = GetMousePosition();
        Vector2f currentMousePos = Application::Get().GetRenderer()->ScreenToLogical(rawMouse);
        if (currentMousePos.x != previousMousePos.x || currentMousePos.y != previousMousePos.y)
        {
            MouseMovedEvent e(currentMousePos.x, currentMousePos.y);
            eventBus.Publish(e);
            previousMousePos = currentMousePos;
        }

        // 4. --- MOUSE SCROLL ---
        float scrollDelta = GetMouseWheel();
        if (std::abs(scrollDelta) > 0.001f)
        {
            accumulatedScroll += scrollDelta;
            MouseScrolledEvent e(accumulatedScroll, scrollDelta);
            eventBus.Publish(e);
        }
    }
}