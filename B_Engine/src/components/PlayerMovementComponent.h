#pragma once

#include <array>
#include <string>

#include "Component.h"
#include "../inputs/KeyCodes.h"

namespace Engine
{
    class AnimatedSpriteComponent;

    struct MovementKeys
    {
        Key up = Key::W;
        Key down = Key::S;
        Key left = Key::A;
        Key right = Key::D;
    };

    /// Simple kinematic movement component (Non-Physics).
    class PlayerMovementComponent : public Component
    {
    public:
        PlayerMovementComponent(float speed = 200.0f, MovementKeys customKeys = {});
        ~PlayerMovementComponent() override = default;

        void Start() override;
        void Update(float deltaTime) override;

        std::string ToString() const override;

        void SetKeys(const MovementKeys& newKeys) { keys = newKeys; }

    private:
        AnimatedSpriteComponent* animator = nullptr;
        float speed;
        MovementKeys keys;
    };
}