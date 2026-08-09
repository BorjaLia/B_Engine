#include "PlayerMovement3DComponent.h"

#include "../scenes/Node.h"
#include "../core/Application.h"
#include "../utils/StringHash.h"
#include "CameraComponent.h"

namespace Engine
{
    PlayerMovement3DComponent::PlayerMovement3DComponent(CameraComponent* camera, float walkSpeed, float runMult)
        : mainCamera(camera), walkSpeed(walkSpeed), runMult(runMult)
    {
    }

    void PlayerMovement3DComponent::Update(float deltaTime)
    {
        if (!owner) return;

        auto& mapper = Application::Get().GetInputManager();

        isRunning = mapper.IsActionPressed(Hash::GetHash("Game_Run"));

        // Get Input Axes
        float moveX = mapper.GetAxis(Hash::GetHash("Game_MoveX"));
        float moveZ = mapper.GetAxis(Hash::GetHash("Game_MoveZ"));

        float vertical = 0.0f;

        if (mapper.IsActionPressed(Hash::GetHash("Game_Jump"))) vertical++;
        if(mapper.IsActionPressed(Hash::GetHash("Game_Crouch"))) vertical--;

        currentInput = { moveX, moveZ };

        if (moveX == 0.0f && moveZ == 0.0f && vertical == 0.0f)
        {
            currentVelocity = { 0.0f, 0.0f, 0.0f };
            return; // No input this frame
        }

        Vector3f currentPos = owner->transform.GetPosition();

        Vector3f forward, right, up;

        if (mainCamera && mainCamera->GetOwner())
        {
            // Move relative to the camera's orientation
            forward = mainCamera->GetOwner()->transform.GetForward();
            right = mainCamera->GetOwner()->transform.GetRight();
            up = mainCamera->GetOwner()->transform.GetUp();
        }
        else
        {
            // Fallback to local orientation if no camera is found
            forward = owner->transform.GetForward();
            right = owner->transform.GetRight();
            up = owner->transform.GetUp();
        }

        // If pressing 'W' (positive Z input) makes you go backwards, flip the sign here (-moveZ).
        Vector3f movement = (forward * moveZ) + (right * -moveX) + (up * vertical);

        if (movement.Magnitude() > 1.0f)
        {
            movement = movement.Normalized();
        }

        currentVelocity = movement * walkSpeed * (isRunning ? runMult : 1.0f);

        owner->transform.SetPosition(currentPos + (currentVelocity * deltaTime));
    }
}