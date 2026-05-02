#include "RigidBodyComponent.h"

#include <cmath>
#include <algorithm>

#include "../scenes/Node.h"
#include "TransformComponent.h"

namespace Engine
{
    // Anonymous namespace to keep math constants private to this file
    namespace
    {
        constexpr float DEG2RAD = 3.14159265f / 180.0f;
        constexpr float RAD2DEG = 180.0f / 3.14159265f;
    }

    RigidBodyComponent::RigidBodyComponent(BodyType initialType, bool lockRotation)
        : type(initialType), lockRotation(lockRotation)
    {
    }

    void RigidBodyComponent::FixedUpdate(float fixedDeltaTime)
    {
        if (owner == nullptr || owner->transform == nullptr) return;

        if (type == BodyType::Static) return;

        // --- KINEMATIC: user-driven velocity only, no gravity ---
        if (type == BodyType::Kinematic)
        {
            Vector2f pos = owner->transform->GetPosition();
            pos.x += velocity.x * fixedDeltaTime;
            pos.y += velocity.y * fixedDeltaTime;
            owner->transform->SetPosition(pos);

            if (!lockRotation)
            {
                float rot = owner->transform->GetRotation();
                rot -= angularVelocity * RAD2DEG * fixedDeltaTime;
                owner->transform->SetRotation(rot);
            }
            return;
        }

        // --- DYNAMIC: full Newton integration ---

        // 1. Gravity
        const float GRAVITY_Y = -981.0f; // px/s^2, Y-up
        acceleration.y += GRAVITY_Y * gravityScale;

        // 2. Linear damping (exponential drag — framerate independent)
        float linearRetain = 1.0f - std::min(linearDamping, 1.0f);
        velocity.x += acceleration.x * fixedDeltaTime;
        velocity.y += acceleration.y * fixedDeltaTime;
        velocity.x *= linearRetain;
        velocity.y *= linearRetain;

        Vector2f pos = owner->transform->GetPosition();
        pos.x += velocity.x * fixedDeltaTime;
        pos.y += velocity.y * fixedDeltaTime;
        owner->transform->SetPosition(pos);

        // 3. Angular integration
        if (!lockRotation)
        {
            // Angular damping — lets collision-induced spin decay over time
            float angularRetain = 1.0f - std::min(angularDamping, 1.0f);
            angularVelocity += angularAcceleration * fixedDeltaTime;
            angularVelocity *= angularRetain;

            float rot = owner->transform->GetRotation();

            // angularVelocity is in rad/s (from the impulse solver) -> convert to deg for the transform
            rot += angularVelocity * RAD2DEG * fixedDeltaTime;
            owner->transform->SetRotation(rot);
        }

        // 4. Clear accumulators
        acceleration = { 0.0f, 0.0f };
        angularAcceleration = 0.0f;
    }

    void RigidBodyComponent::AddLocalForce(const Vector2f& localForce)
    {
        if (type != BodyType::Dynamic || owner == nullptr || owner->transform == nullptr) return;

        float rad = owner->transform->GetRotation() * DEG2RAD;
        float cosA = std::cos(rad);
        float sinA = std::sin(rad);

        Vector2f globalForce(
            localForce.x * cosA - localForce.y * sinA,
            localForce.x * sinA + localForce.y * cosA
        );
        AddForce(globalForce);
    }

    void RigidBodyComponent::AddForce(const Vector2f& force)
    {
        if (type != BodyType::Dynamic) return;
        // a = F / m
        acceleration.x += force.x / mass;
        acceleration.y += force.y / mass;
    }

    void RigidBodyComponent::AddTorque(float torque)
    {
        if (type != BodyType::Dynamic || lockRotation) return;
        // Newton's 2nd law for rotation: alpha = tau / I
        angularAcceleration += torque / inertia;
    }

    void RigidBodyComponent::ComputeBoxInertia(Vector2f size)
    {
        if (type == BodyType::Dynamic && !lockRotation)
        {
            float newInertia = (mass / 12.0f) * (size.x * size.x + size.y * size.y);
            SetInertia(newInertia);
        }
    }

    void RigidBodyComponent::SetMassAndBoxInertia(float newMass, Vector2f size)
    {
        SetMass(newMass);
        ComputeBoxInertia(size);
    }
}