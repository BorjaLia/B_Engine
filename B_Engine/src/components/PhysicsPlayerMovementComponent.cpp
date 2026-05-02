#include "PhysicsPlayerMovementComponent.h"

#include <cmath>

#include "../scenes/Node.h"
#include "RigidBodyComponent.h"
#include "AnimatedSpriteComponent.h"
#include "../core/Application.h" 
#include "../utils/StringHash.h" 

namespace Engine
{
    PhysicsPlayerController::PhysicsPlayerController(float walkSpd, float runSpd, float jumpFrc)
        : walkSpeed(walkSpd), runSpeed(runSpd), jumpForce(jumpFrc)
    {
    }

    void PhysicsPlayerController::Start()
    {
        if (owner)
        {
            rigidBody = owner->GetComponent<RigidBodyComponent>();
            animator = owner->GetComponent<AnimatedSpriteComponent>();
        }
    }

    void PhysicsPlayerController::FixedUpdate(float /*fixedDeltaTime*/)
    {
        if (!rigidBody) return;

        auto& mapper = Application::Get().GetInputManager();

        float moveX = mapper.GetAxis(Hash::GetHash("Game_MoveX"));
        float rotX = mapper.GetAxis(Hash::GetHash("Game_RotX"));
        isRunning = mapper.IsActionPressed(Hash::GetHash("Game_Run"));

        float speed = isRunning ? runSpeed : walkSpeed;
        Vector2f vel = rigidBody->GetVelocity();
        vel.x = 0.0f;

        // --- SMART CONTROLLER MAGIC ---
        // We only apply horizontal velocity if we are NOT hitting a wall in that direction.
        if (moveX > 0.0f && !touchingRightWall) vel.x = speed * moveX;
        else if (moveX < 0.0f && !touchingLeftWall) vel.x = speed * moveX;

        if (std::abs(rotX) > 0.0f) rigidBody->AddTorque(torqueForce * rotX);

        // --- DIRECT & DETERMINISTIC JUMP ---
        if (mapper.IsActionJustPressed(Hash::GetHash("Game_Jump")))
        {
            if (isGrounded)
            {
                vel.y = jumpForce;
            }
        }

        rigidBody->SetVelocity(vel);

        // --- SENSOR RESET ---
        // We turn off flags at the end of the frame. The physics engine will run
        // OnCollision milliseconds later and turn them back on if we are still touching.
        isGrounded = false;
        touchingLeftWall = false;
        touchingRightWall = false;
    }

    void PhysicsPlayerController::OnCollision(const Physics::CollisionManifold& hit)
    {
        // Y-Up: Normal pointing upwards (Floor)
        if (hit.normal.y > 0.5f) isGrounded = true;

        // X-Right: Normal points right, so the wall is to our LEFT
        if (hit.normal.x > 0.1f) touchingLeftWall = true;

        // X-Left: Normal points left, so the wall is to our RIGHT
        if (hit.normal.x < -0.1f) touchingRightWall = true;
    }
}