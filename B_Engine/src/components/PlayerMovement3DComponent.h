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
        PlayerMovement3DComponent(CameraComponent* camera, float walkSpeed = 15.0f, float runMult = 2.5f);
        ~PlayerMovement3DComponent() override = default;

        void Update(float deltaTime) override;

        void SetSpeed(float newSpeed) { walkSpeed = newSpeed; }
        float GetSpeed() const { return walkSpeed; }

        Vector2f GetCurrentInput() const { return currentInput; }
        Vector3f GetCurrentVelocity() const { return currentVelocity; }

    private:
        float walkSpeed;
        float runMult;
        CameraComponent* mainCamera = nullptr;

        Vector2f currentInput;
        Vector3f currentVelocity;
    };
}