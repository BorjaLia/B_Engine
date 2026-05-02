#include "InputManager.h"

#include "../core/Application.h" 

namespace Engine
{
    void InputManager::Initialize(IInput* inputWindow)
    {
        rawInput = inputWindow;

        // The injector does not require init right now, it is loaded on-demand
        mapper.Initialize();
        logger.Initialize();
    }

    void InputManager::Update(EventBus& eventBus)
    {
        if (!rawInput) return;

        // Read hardware (this will fill the pending queues inside the Mapper)
        rawInput->Update(eventBus);
    }

    void InputManager::FixedUpdate()
    {
        // 1. If Replay is active, inject "fake" inputs into the queues
        injector.Update();

        // 2. Freeze the physical snapshot (Applies queues -> actionStates)
        mapper.ApplyPhysicalSnapshot();
    }

    void InputManager::PostFixedUpdate()
    {
        // 3. Cleanup to ensure a JustPressed flag doesn't last for 2 physics frames
        mapper.ClearPhysicalSnapshot();
    }
}