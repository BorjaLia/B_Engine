#pragma once

#include "Component.h"
#include "../inputs/KeyCodes.h"
#include "../math/Vector3.h"

namespace Engine
{
    class CameraComponent;

    /// Basic logic script to move a Node in 3D space based on Input.
    /// @ingroup Components
    class PlayerMovement3DComponent : public Component
    {
    public:
        PlayerMovement3DComponent(CameraComponent* camera, float speed = 10.0f);
        ~PlayerMovement3DComponent() override = default;

        void Update(float deltaTime) override;

        void SetSpeed(float newSpeed) { moveSpeed = newSpeed; }
        float GetSpeed() const { return moveSpeed; }

        Vector2f GetCurrentInput() const { return currentInput; }
        Vector3f GetCurrentVelocity() const { return currentVelocity; }

    private:
        float moveSpeed;
        CameraComponent* mainCamera = nullptr;

        Vector2f currentInput;
        Vector3f currentVelocity;
    };
}