#include "PlatformScript.h"

#include "scenes/Node.h"
#include "components/RigidBodyComponent.h"

void PlatformScript::OnStart()
{
    body = owner->GetComponent<Engine::RigidBodyComponent>();
    if (body)
    {
        body->SetGravityScale(0.0f);
    }
}

void PlatformScript::OnUpdate(float deltaTime)
{
    (void)deltaTime;

    if (body)
    {
        body->SetVelocity({ 0.0f, 0.0f });
        body->SetAngularVelocity(0.0f);
        owner->transform->SetPosition(startPos);
        owner->transform->SetRotation(0.0f);
    }
}

void PlatformScript::OnFixedUpdate(float fixedDeltaTime)
{
    (void)fixedDeltaTime;

    if (body)
    {
        body->SetVelocity({ 0.0f, 0.0f });
        body->SetAngularVelocity(0.0f);
        owner->transform->SetPosition(startPos);
        owner->transform->SetRotation(0.0f);
    }
}