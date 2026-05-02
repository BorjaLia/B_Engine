#pragma once

#include "scripts/Script.h"

namespace Engine { class RigidBodyComponent; }

/// Simple patrol AI that moves horizontally between two boundaries.
class EnemyScript : public Engine::Script
{
public:
    EnemyScript(float leftBound, float rightBound, float speed = 150.0f);
    ~EnemyScript() override = default;

    void OnStart() override;
    void OnFixedUpdate(float fixedDeltaTime) override;

private:
    Engine::RigidBodyComponent* body = nullptr;
    float leftX, rightX, speed;
    bool movingRight = true;
};