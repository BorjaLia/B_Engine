#pragma once

#include "Component.h"
#include "../inputs/KeyCodes.h"
#include "../physics/ICollisionListener.h"

namespace Engine
{
    class RigidBodyComponent;
    class AnimatedSpriteComponent;

    /// Specialized controller component that handles physics-based player movement.
    class PhysicsPlayerController : public Component, public ICollisionListener
    {
    public:
        PhysicsPlayerController(float walkSpd = 300.0f, float runSpd = 500.0f, float jumpFrc = 500.0f);
        ~PhysicsPlayerController() override = default;

        void Start() override;
        void FixedUpdate(float fixedDeltaTime) override;
        void OnCollision(const Physics::CollisionManifold& hit) override;

        void SetTorqueForce(float f) { torqueForce = f; }

        float GetWalkSpeed() const { return walkSpeed; }
        float GetRunSpeed() const { return runSpeed; }

        bool IsRunning() const { return isRunning; }

        void SetGrounded(bool setGrounded) { isGrounded = setGrounded; }
        bool GetGrounded() const { return isGrounded; }

    private:
        RigidBodyComponent* rigidBody = nullptr;
        AnimatedSpriteComponent* animator = nullptr;

        float walkSpeed = 300.0f;
        float runSpeed = 500.0f;
        float jumpForce = 500.0f;
        float torqueForce = 3000.0f;

        bool isGrounded = true;
        bool touchingLeftWall = false;
        bool touchingRightWall = false;

        // Cached state so other scripts (like PlayerScript) can query animation states
        bool isRunning = false;
    };
}