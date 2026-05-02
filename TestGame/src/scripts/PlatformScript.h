#pragma once

#include "scripts/Script.h"
#include "utils/Types.h"

namespace Engine
{
    class RigidBodyComponent;
}

/// A script to strictly enforce a static platform's position, overriding physics impulses.
class PlatformScript : public Engine::Script
{
public:
    PlatformScript(Engine::Vector2f startPos) : startPos(startPos) {}
    ~PlatformScript() override = default;

    void OnStart() override;
    void OnUpdate(float deltaTime) override;
    void OnFixedUpdate(float fixedDeltaTime) override;

private:
    Engine::RigidBodyComponent* body = nullptr;
    Engine::Vector2f startPos;
};