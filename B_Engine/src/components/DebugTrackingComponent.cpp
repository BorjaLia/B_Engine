#include "DebugTrackingComponent.h"

#include "../scenes/Node.h"
#include "../core/Application.h"
#include "../debug/Debug.h"

namespace Engine
{
    DebugTrackingComponent::DebugTrackingComponent()
        : lastPosition(0.0f, 0.0f), wasMoving(false)
    {
    }

    void DebugTrackingComponent::Update(float /*deltaTime*/)
    {
        if (!Application::Get().IsDebugMode()) return;
        if (owner == nullptr) return;

        Vector2f currentPos = owner->GetGlobalPosition();

        bool isMovingNow = (currentPos.x != lastPosition.x || currentPos.y != lastPosition.y);

        if (isMovingNow)
        {
            wasMoving = true;
        }
        else if (!isMovingNow && wasMoving)
        {
            // Node just stopped moving
            wasMoving = false;
        }
        lastPosition = currentPos;
    }

    std::string DebugTrackingComponent::ToString() const
    {
        return "DebugTrackingComponent [WasMoving: " + std::string(wasMoving ? "True" : "False") + "]";
    }
}