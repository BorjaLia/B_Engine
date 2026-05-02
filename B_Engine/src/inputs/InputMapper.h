#pragma once

#include <unordered_map>
#include <vector>
#include <cstdint>

#include "../events/EventBus.h"
#include "../events/InputEvents.h" 
#include "KeyCodes.h"

namespace Engine
{
    /// Internal structure binding an action to an analog scale.
    struct ActionBinding
    {
        uint32_t actionHash;
        float scale; // e.g. 1.0f for "Right", -1.0f for "Left"
    };

    /// Translates raw hardware input keys into semantic game actions.
    class InputMapper
    {
    public:
        InputMapper() = default;
        ~InputMapper();

        void Initialize();

#pragma region Physical Orchestration
        /// Commits pending input states. Called at the START of FixedUpdate.
        void ApplyPhysicalSnapshot();

        /// Clears temporary snapshot buffers. Called at the END of FixedUpdate.
        void ClearPhysicalSnapshot();
#pragma endregion

#pragma region Hardware Binding
        void BindAction(uint32_t actionHash, Key key, ActionCategory category = ActionCategory::Gameplay) { BindAxis(actionHash, key, 1.0f, category); }
        void BindAxis(uint32_t actionHash, Key key, float scale, ActionCategory category = ActionCategory::Gameplay);

        void BindAction(uint32_t actionHash, MouseButton button, ActionCategory category = ActionCategory::Gameplay) { BindAxis(actionHash, button, 1.0f, category); }
        void BindAxis(uint32_t actionHash, MouseButton button, float scale, ActionCategory category = ActionCategory::Gameplay);

        void SetAbsoluteAxis(uint32_t actionHash, float absoluteValue, ActionCategory category = ActionCategory::Gameplay);
#pragma endregion

#pragma region Action Polling
        float GetAxis(uint32_t actionHash);
        bool IsActionPressed(uint32_t actionHash);
        bool IsActionJustPressed(uint32_t actionHash);
        bool IsActionJustReleased(uint32_t actionHash);
#pragma endregion

    private:
        // Live state of all registered actions
        std::unordered_map<uint32_t, ActionState> actionStates;

        // Visual World Buffers (Temporary Memory)
        std::unordered_map<uint32_t, float> liveAxisValues;
        std::vector<uint32_t> pendingJustPressed;
        std::vector<uint32_t> pendingJustReleased;

        // Keyboard Hardware Map
        std::unordered_map<Key, std::vector<ActionBinding>> keyBindings;
        std::unordered_map<Key, bool> rawKeyStates;
        uint32_t keyPressedSubId = 0;
        uint32_t keyReleasedSubId = 0;

        // Mouse Hardware Map
        std::unordered_map<MouseButton, std::vector<ActionBinding>> mouseBindings;
        std::unordered_map<MouseButton, bool> rawMouseStates;
        uint32_t mousePressedSubId = 0;
        uint32_t mouseReleasedSubId = 0;

        uint32_t mouseMovedSubId = 0;
        uint32_t mouseScrolledSubId = 0;

        void OnKeyPressed(KeyPressedEvent& e);
        void OnKeyReleased(KeyReleasedEvent& e);
        void OnMousePressed(MouseButtonPressedEvent& e);
        void OnMouseReleased(MouseButtonReleasedEvent& e);
        void OnMouseMoved(MouseMovedEvent& e);
        void OnMouseScrolled(MouseScrolledEvent& e);

        void UpdateActionValue(uint32_t actionHash, float deltaValue);
    };
}