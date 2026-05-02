#pragma once

#include <cstdint>

#include "scripts/Script.h"

namespace Engine
{
    class CameraComponent;
    class ActionChangedEvent;
}

/// Tracks the mouse position in world space and responds to interact/spawn clicks.
class MouseScript : public Engine::Script
{
public:
    MouseScript(Engine::CameraComponent* camera = nullptr);
    ~MouseScript() override;

    void OnStart() override;
    void OnUpdate(float deltaTime) override;
    void OnActionChanged(Engine::ActionChangedEvent& e);

private:
    Engine::CameraComponent* camera = nullptr;
    uint32_t actionSubId = 0;
};