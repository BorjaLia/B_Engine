#pragma once

#include <algorithm>

#include "Component.h"
#include "../utils/Types.h"

namespace Engine
{
    enum class BodyType
    {
        Static,    // Walls/floors. Infinite mass. Never moves. (CPU cost: ~0)
        Kinematic, // Moving platforms. Moved by code, ignores gravity.
        Dynamic    // Player/boxes. Full Newtonian physics.
    };

    /// Attaches Newtonian physics properties to a Node.
    class RigidBodyComponent : public Component
    {
    public:
        RigidBodyComponent(BodyType initialType = BodyType::Dynamic, bool lockRotation = false);
        ~RigidBodyComponent() override = default;

        void FixedUpdate(float fixedDeltaTime) override;

        void AddLocalForce(const Vector2f& localForce);
        void AddForce(const Vector2f& force);
        void AddTorque(float torque);

        void ComputeBoxInertia(Vector2f size);
        void SetMassAndBoxInertia(float newMass, Vector2f size);

        BodyType GetType()             const { return type; }
        void     SetType(BodyType t) { type = t; }

        Vector2f GetVelocity()         const { return velocity; }
        void     SetVelocity(const Vector2f& v) { velocity = v; }

        float    GetMass()             const { return mass; }
        void     SetMass(float m) { if (m > 0.001f) mass = m; }

        float    GetAngularVelocity()  const { return angularVelocity; }
        void     SetAngularVelocity(float v) { angularVelocity = v; }

        float    GetInertia()          const { return inertia; }
        void     SetInertia(float i) { if (i > 0.001f) inertia = i; }

        bool     GetLockRotation()     const { return lockRotation; }
        void     SetLockRotation(bool lock)
        {
            lockRotation = lock;
            if (lock) { angularVelocity = 0.0f; angularAcceleration = 0.0f; }
        }

        float    GetFriction()         const { return friction; }
        void     SetFriction(float f) { friction = f; }

        float    GetRestitution()      const { return restitution; }
        void     SetRestitution(float r) { restitution = r; }

        float    GetLinearDamping()    const { return linearDamping; }
        void     SetLinearDamping(float d) { linearDamping = std::max(0.0f, d); }

        float    GetAngularDamping()   const { return angularDamping; }
        void     SetAngularDamping(float d) { angularDamping = std::max(0.0f, d); }

        float    GetGravityScale()     const { return gravityScale; }
        void     SetGravityScale(float s) { gravityScale = s; }

    private:
        BodyType type;

        // Linear physics
        Vector2f velocity = { 0.0f, 0.0f };
        Vector2f acceleration = { 0.0f, 0.0f };
        float mass = 1.0f;

        // Angular physics
        float angularVelocity = 0.0f;
        float angularAcceleration = 0.0f;
        float inertia = 1.0f; // Rotational mass (resistance to spinning)

        // Damping — exponential drag applied each fixed step
        float linearDamping = 0.0f;
        float angularDamping = 0.02f;

        // Config
        float gravityScale = 1.0f;
        bool  lockRotation = false;

        float friction = 0.5f;
        float restitution = 0.2f;
    };
}