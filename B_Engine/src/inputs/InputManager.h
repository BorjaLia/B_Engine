#pragma once

#include "InputMapper.h"
#include "InputLogger.h"
#include "InputInjector.h"

#include "../events/EventBus.h"
#include "../inputs/InputBase.h"
#include "../utils/StringHash.h"

namespace Engine
{
    /// @defgroup Inputs Input System
    /// @brief Action mapping, hardware polling, and the deterministic Replay Injector.
    ///
    /// High-level facade that coordinates the InputMapper, InputLogger, and InputInjector.
    /// @ingroup Inputs
    class InputManager
    {
    public:
        InputManager(const InputManager&) = delete;
        InputManager& operator=(const InputManager&) = delete;
        InputManager(InputManager&&) = delete;
        InputManager& operator=(InputManager&&) = delete;

        /// Orchestrates the lifecycle of the input subsystems.
        void Initialize(InputBase* inputWindow);

        /// Polls raw hardware updates (Visual Loop).
        void Update(EventBus& eventBus);

        /// Injects inputs and finalizes mapped action states (Physics Loop).
        void FixedUpdate();

        /// Clears temporary snapshot flags.
        void PostFixedUpdate();

#pragma region Configuration Facade
        void BindAction(uint32_t actionHash, Key key, ActionCategory category = ActionCategory::Gameplay) { mapper.BindAction(actionHash, key, category); }
        void BindAxis(uint32_t actionHash, Key key, float scale, ActionCategory category = ActionCategory::Gameplay) { mapper.BindAxis(actionHash, key, scale, category); }

        void BindAction(uint32_t actionHash, MouseButton button, ActionCategory category = ActionCategory::Gameplay) { mapper.BindAction(actionHash, button, category); }
        void BindAxis(uint32_t actionHash, MouseButton button, float scale, ActionCategory category = ActionCategory::Gameplay) { mapper.BindAxis(actionHash, button, scale, category); }
#pragma endregion

#pragma region Polling API
        float GetAxis(uint32_t actionHash) { return mapper.GetAxis(actionHash); }
        bool IsActionPressed(uint32_t actionHash) { return mapper.IsActionPressed(actionHash); }
        bool IsActionJustPressed(uint32_t actionHash) { return mapper.IsActionJustPressed(actionHash); }
        bool IsActionJustReleased(uint32_t actionHash) { return mapper.IsActionJustReleased(actionHash); }

        void SetAbsoluteAxis(uint32_t actionHash, float absoluteValue, ActionCategory category = ActionCategory::Gameplay)
        {
            mapper.SetAbsoluteAxis(actionHash, absoluteValue, category);
        }

        Vector2f GetMousePosition()
        {
            return { mapper.GetAxis(Hash::GetHash("Pointer_X")), mapper.GetAxis(Hash::GetHash("Pointer_Y")) };
        }
#pragma endregion

        InputLogger& GetLogger() { return logger; }
        InputInjector& GetInjector() { return injector; }

    private:
        friend class Application;

        InputManager() = default;
        ~InputManager() = default;

        InputBase* rawInput = nullptr;

        InputMapper mapper;
        InputLogger logger;
        InputInjector injector;
    };
}