#include "EnemyScript.h"

#include "scenes/Node.h"
#include "components/RigidBodyComponent.h"
#include "components/TriggerAreaComponent.h"
#include "debug/Debug.h"

#include "GameManager.h"

EnemyScript::EnemyScript(float leftBound, float rightBound, float speed)
    : leftX(leftBound), rightX(rightBound), speed(speed)
{
}

void EnemyScript::OnStart()
{
    body = owner->GetComponent<Engine::RigidBodyComponent>();

    auto* trigger = owner->GetComponent<Engine::TriggerAreaComponent>();
    if (trigger)
    {
        trigger->ConnectTriggerEnter([this](Engine::Node* other)
            {
                if (other->name == "Player" && GameManager::Get())
                {
                    GameManager::Get()->Lose();
                }
            });
    }

    if (GameManager::Get())
    {
        speed = GameManager::Get()->GetRandom().GetFloat(50.0f, 250.0f);
        ENGINE_INFO("Enemy speed set to: {}", speed);
    }
}

void EnemyScript::OnFixedUpdate(float fixedDeltaTime)
{
    (void)fixedDeltaTime;

    if (!body) return;

    if (GameManager::Get() && (GameManager::Get()->IsPaused() || GameManager::Get()->IsOver()))
    {
        body->SetVelocity({ 0.0f, body->GetVelocity().y });
        return;
    }

    Engine::Vector2f vel = body->GetVelocity();
    vel.x = movingRight ? speed : -speed;
    body->SetVelocity(vel);

    float x = owner->transform.GetGlobalPosition().x;
    if (movingRight && x >= rightX) movingRight = false;
    if (!movingRight && x <= leftX)  movingRight = true;
}