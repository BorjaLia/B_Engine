#include "PlayerMovementComponent.h"

#include <sstream>

#include "../scenes/Node.h"
#include "TransformComponent.h"
#include "AnimatedSpriteComponent.h"
#include "../events/GameEvents.h" 
#include "../core/Application.h" 

namespace Engine
{
    PlayerMovementComponent::PlayerMovementComponent(float speed, MovementKeys customKeys)
        : speed(speed), keys(customKeys)
    {
    }

    void PlayerMovementComponent::Start()
    {
        if (owner)
        {
            animator = owner->GetComponent<AnimatedSpriteComponent>();
        }
    }

    void PlayerMovementComponent::Update(float deltaTime)
    {
        (void)deltaTime;
    }

    std::string PlayerMovementComponent::ToString() const
    {
        std::stringstream ss;
        ss << "PlayerMovementComponent [Speed: " << speed << "]";
        return ss.str();
    }
}