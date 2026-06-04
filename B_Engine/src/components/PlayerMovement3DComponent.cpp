#include "PlayerMovement3DComponent.h"

#include "../scenes/Node.h"
#include "../core/Application.h"
#include "../utils/StringHash.h"
#include "CameraComponent.h"

namespace Engine
{
    PlayerMovement3DComponent::PlayerMovement3DComponent(CameraComponent* camera, float speed)
        : mainCamera(camera), moveSpeed(speed)
    {
    }

    void PlayerMovement3DComponent::Update(float deltaTime)
    {
        if (!owner) return;

        auto& mapper = Application::Get().GetInputManager();

        // Get Input Axes
        float moveX = mapper.GetAxis(Hash::GetHash("Game_MoveX"));
        float moveY = mapper.GetAxis(Hash::GetHash("Game_MoveY"));

        currentInput = { moveX, moveY };

        if (moveX == 0.0f && moveY == 0.0f)
        {
            currentVelocity = { 0.0f, 0.0f, 0.0f };
            return; // No input this frame
        }

        Vector3f currentPos = owner->transform.GetPosition();

        Vector3f forward, right;

        if (mainCamera && mainCamera->GetOwner())
        {
            // Move relative to the camera's orientation
            forward = mainCamera->GetOwner()->transform.GetForward();
            right = mainCamera->GetOwner()->transform.GetRight();
        }
        else
        {
            // Fallback to local orientation if no camera is found
            forward = owner->transform.GetForward();
            right = owner->transform.GetRight();
        }

        forward.y = 0.0f;
        right.y = 0.0f;

        forward.Normalize();
        right.Normalize();

        // Note: moveY is mapped to the 'Forward' axis (Z in most engines).
        // If pressing 'W' (positive Y input) makes you go backwards, flip the sign here (-moveY).
        Vector3f movement = (forward * moveY) + (right * -moveX);

        if (movement.Magnitude() > 1.0f)
        {
            movement = movement.Normalized();
        }

        currentVelocity = movement * moveSpeed;

        owner->transform.SetPosition(currentPos + (currentVelocity * deltaTime));
    }
}