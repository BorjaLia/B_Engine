#include "PlayerMovement3DComponent.h"

#include "../scenes/Node.h"
#include "../core/Application.h"
#include "../utils/StringHash.h"

namespace Engine
{
    PlayerMovement3DComponent::PlayerMovement3DComponent(float speed)
        : moveSpeed(speed)
    {
    }

    void PlayerMovement3DComponent::Update(float deltaTime)
    {
        if (!owner) return;

        auto& mapper = Application::Get().GetInputManager();

        // Get Input Axes (Assuming WASD are mapped to MoveX and MoveY)
        float moveX = mapper.GetAxis(Hash::GetHash("Game_MoveX"));
        float moveY = mapper.GetAxis(Hash::GetHash("Game_MoveY"));

        if (moveX == 0.0f && moveY == 0.0f) return; // No input this frame

        // 1. Get current position
        Vector3f currentPos = owner->transform.GetPosition();

        // 2. Get directional vectors based on current rotation
        // In a typical 3D game, MoveY (W/S keys) moves along the Forward axis, 
        // and MoveX (A/D keys) moves along the Right axis.
        Vector3f forward = owner->transform.GetForward();
        Vector3f right = owner->transform.GetRight();

        // Optional: If you want an FPS-style camera where moving forward doesn't make you fly up/down,
        // you would flatten the 'y' coordinate of the forward vector and re-normalize it here.
        // forward.y = 0; forward.Normalized();

        // 3. Calculate velocity vector
        // velocity = (forward * inputY) + (right * inputX)
        Vector3f movement = (forward * moveY) + (right * moveX);

        // Normalize to prevent faster diagonal movement
        if (movement.Magnitude() > 1.0f)
        {
            movement = movement.Normalized();
        }

        // 4. Apply movement
        owner->transform.SetPosition(currentPos + (movement * moveSpeed * deltaTime));
    }
}