#include "InputMapper.h"

#include <cmath>
#include "../core/Application.h" 
#include "../utils/StringHash.h"

namespace Engine
{
    InputMapper::~InputMapper()
    {
        auto& bus = Application::Get().GetEventBus();
        bus.Unsubscribe(KeyPressedEvent::GetStaticType(), keyPressedSubId);
        bus.Unsubscribe(KeyReleasedEvent::GetStaticType(), keyReleasedSubId);
        bus.Unsubscribe(MouseButtonPressedEvent::GetStaticType(), mousePressedSubId);
        bus.Unsubscribe(MouseButtonReleasedEvent::GetStaticType(), mouseReleasedSubId);
        bus.Unsubscribe(MouseMovedEvent::GetStaticType(), mouseMovedSubId);
        bus.Unsubscribe(MouseScrolledEvent::GetStaticType(), mouseScrolledSubId);
        bus.Unsubscribe(ReplayStateEvent::GetStaticType(), replayStateSubId);
    }

    void InputMapper::Initialize()
    {
        auto& bus = Application::Get().GetEventBus();

        keyPressedSubId = bus.Subscribe<KeyPressedEvent>(
            [this](KeyPressedEvent& e) { this->OnKeyPressed(e); }
        );
        keyReleasedSubId = bus.Subscribe<KeyReleasedEvent>(
            [this](KeyReleasedEvent& e) { this->OnKeyReleased(e); }
        );

        mousePressedSubId = bus.Subscribe<MouseButtonPressedEvent>(
            [this](MouseButtonPressedEvent& e) { this->OnMousePressed(e); }
        );
        mouseReleasedSubId = bus.Subscribe<MouseButtonReleasedEvent>(
            [this](MouseButtonReleasedEvent& e) { this->OnMouseReleased(e); }
        );
        mouseMovedSubId = bus.Subscribe<MouseMovedEvent>(
            [this](MouseMovedEvent& e) { this->OnMouseMoved(e); }
        );
        mouseScrolledSubId = bus.Subscribe<MouseScrolledEvent>(
            [this](MouseScrolledEvent& e) { this->OnMouseScrolled(e); }
        );

        replayStateSubId = bus.Subscribe<ReplayStateEvent>(
            [this](ReplayStateEvent& e) { this->isReplaying = e.IsPlaying(); }
        );
    }

    void InputMapper::BindAxis(uint32_t actionHash, Key key, float scale, ActionCategory category)
    {
        keyBindings[key].push_back({ actionHash, scale });

        // Initialize state category on first bind
        if (actionStates.find(actionHash) == actionStates.end())
        {
            ActionState newState;
            newState.category = category;
            actionStates[actionHash] = newState;
        }
    }

    void InputMapper::BindAxis(uint32_t actionHash, MouseButton button, float scale, ActionCategory category)
    {
        mouseBindings[button].push_back({ actionHash, scale });

        if (actionStates.find(actionHash) == actionStates.end())
        {
            ActionState newState;
            newState.category = category;
            actionStates[actionHash] = newState;
        }
    }

    void InputMapper::OnKeyPressed(KeyPressedEvent& e)
    {
        if (isReplaying) return;

        Key key = e.GetKey();
        if (rawKeyStates[key]) return;
        rawKeyStates[key] = true;

        if (keyBindings.find(key) != keyBindings.end())
        {
            for (const ActionBinding& binding : keyBindings[key])
            {
                UpdateActionValue(binding.actionHash, binding.scale);
            }
        }
    }

    void InputMapper::OnKeyReleased(KeyReleasedEvent& e)
    {
        if (isReplaying) return;

        Key key = e.GetKey();
        if (!rawKeyStates[key]) return;
        rawKeyStates[key] = false;

        if (keyBindings.find(key) != keyBindings.end())
        {
            for (const ActionBinding& binding : keyBindings[key])
            {
                UpdateActionValue(binding.actionHash, -binding.scale);
            }
        }
    }

    void InputMapper::OnMousePressed(MouseButtonPressedEvent& e)
    {
        if (isReplaying) return;

        MouseButton button = e.GetButton();
        if (rawMouseStates[button]) return;
        rawMouseStates[button] = true;

        if (mouseBindings.find(button) != mouseBindings.end())
        {
            for (const ActionBinding& binding : mouseBindings[button])
            {
                UpdateActionValue(binding.actionHash, binding.scale);
            }
        }
    }

    void InputMapper::OnMouseReleased(MouseButtonReleasedEvent& e)
    {
        if (isReplaying) return;

        MouseButton button = e.GetButton();
        if (!rawMouseStates[button]) return;
        rawMouseStates[button] = false;

        if (mouseBindings.find(button) != mouseBindings.end())
        {
            for (const ActionBinding& binding : mouseBindings[button])
            {
                UpdateActionValue(binding.actionHash, -binding.scale);
            }
        }
    }

    void InputMapper::OnMouseMoved(MouseMovedEvent& e)
    {
        if (isReplaying) return;

        Vector2f currentPos = { e.GetX(), e.GetY() };

        if (isFirstMouseMove)
        {
            lastMousePos = currentPos;
            isFirstMouseMove = false;
        }

        // Accumulate delta in case multiple events fire in a single frame
        currentMouseDelta.x += (currentPos.x - lastMousePos.x);
        currentMouseDelta.y += (currentPos.y - lastMousePos.y);
        lastMousePos = currentPos;

        SetAbsoluteAxis(Hash::GetHash("Pointer_X"), currentPos.x, ActionCategory::Gameplay);
        SetAbsoluteAxis(Hash::GetHash("Pointer_Y"), currentPos.y, ActionCategory::Gameplay);
    }

    void InputMapper::OnMouseScrolled(MouseScrolledEvent& e)
    {
        if (isReplaying) return;

        SetAbsoluteAxis(Hash::GetHash("Pointer_Scroll"), e.GetAbsolute(), ActionCategory::Gameplay);
    }

    void InputMapper::UpdateActionValue(uint32_t actionHash, float deltaValue)
    {
        // Work on live visual memory
        float& liveVal = liveAxisValues[actionHash];
        bool wasLivePressed = (std::abs(liveVal) > 0.001f);

        liveVal += deltaValue;
        if (std::abs(liveVal) < 0.001f) liveVal = 0.0f;

        bool isLivePressed = (std::abs(liveVal) > 0.001f);

        // Queue state transitions
        if (isLivePressed && !wasLivePressed) pendingJustPressed.push_back(actionHash);
        if (!isLivePressed && wasLivePressed) pendingJustReleased.push_back(actionHash);
    }

    void InputMapper::SetAbsoluteAxis(uint32_t actionHash, float absoluteValue, ActionCategory category)
    {
        float& liveVal = liveAxisValues[actionHash];
        bool wasLivePressed = (std::abs(liveVal) > 0.001f);

        if (std::abs(liveVal - absoluteValue) > 0.001f)
        {
            liveVal = absoluteValue;
            actionStates[actionHash].category = category;

            bool isLivePressed = (std::abs(liveVal) > 0.001f);

            if (isLivePressed && !wasLivePressed) pendingJustPressed.push_back(actionHash);
            if (!isLivePressed && wasLivePressed) pendingJustReleased.push_back(actionHash);
        }
    }

    void InputMapper::ClearVisualSnapshot()
    {
        currentMouseDelta = { 0.0f, 0.0f };
    }

    void InputMapper::ApplyPhysicalSnapshot()
    {
        // 1. Apply and publish ALL changes (Axes and Buttons)
        for (const auto& pair : liveAxisValues)
        {
            uint32_t hash = pair.first;
            float newValue = pair.second;
            ActionState& physState = actionStates[hash];

            // If the value changed this tick, update and fire the UNIQUE event
            if (std::abs(physState.value - newValue) > 0.001f)
            {
                physState.value = newValue;
                ActionChangedEvent event(hash, newValue, physState.category);
                Application::Get().GetEventBus().Publish(event);
            }
        }

        // 2. Update polling booleans
        for (uint32_t hash : pendingJustPressed)
        {
            actionStates[hash].wasPressed = false;
            actionStates[hash].isPressed = true;
        }

        for (uint32_t hash : pendingJustReleased)
        {
            actionStates[hash].wasPressed = true;
            actionStates[hash].isPressed = false;
        }

        pendingJustPressed.clear();
        pendingJustReleased.clear();
    }

    void InputMapper::ClearPhysicalSnapshot()
    {
        for (auto& pair : actionStates)
        {
            pair.second.wasPressed = pair.second.isPressed;
        }
    }

    float InputMapper::GetAxis(uint32_t actionHash)
    {
        auto it = actionStates.find(actionHash);
        return it != actionStates.end() ? it->second.value : 0.0f;
    }

    bool InputMapper::IsActionPressed(uint32_t actionHash)
    {
        auto it = actionStates.find(actionHash);
        return it != actionStates.end() ? it->second.isPressed : false;
    }

    bool InputMapper::IsActionJustPressed(uint32_t actionHash)
    {
        auto it = actionStates.find(actionHash);
        return it != actionStates.end() ? (it->second.isPressed && !it->second.wasPressed) : false;
    }

    bool InputMapper::IsActionJustReleased(uint32_t actionHash)
    {
        auto it = actionStates.find(actionHash);
        return it != actionStates.end() ? (!it->second.isPressed && it->second.wasPressed) : false;
    }
}