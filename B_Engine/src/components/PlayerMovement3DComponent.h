#pragma once

#include "Component.h"
#include "../inputs/KeyCodes.h"
#include "../math/Vector3.h"

namespace Engine
{
    /// Basic logic script to move a Node in 3D space based on Input.
    /// @ingroup Components
    class PlayerMovement3DComponent : public Component
    {
    public:
        PlayerMovement3DComponent(float speed = 10.0f);
        ~PlayerMovement3DComponent() override = default;

        void Update(float deltaTime) override;

        void SetSpeed(float newSpeed) { moveSpeed = newSpeed; }
        float GetSpeed() const { return moveSpeed; }

    private:
        float moveSpeed;
    };
}