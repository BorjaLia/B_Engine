#include "FollowComponent.h"

#include <cmath>
#include <algorithm> 

#include "../core/Application.h" 
#include "../scenes/Node.h"
#include "TransformComponent.h"
#include "RigidBodyComponent.h"
#include "../core/interfaces/IRenderer.h"

namespace Engine
{
    FollowComponent::FollowComponent(FollowMode followMode, float speed)
        : mode(followMode), speed(speed)
    {
    }

#pragma region Target Reading
    Vector2f FollowComponent::CalculateTargetLookahead() const
    {
        if (lookaheadMultiplier <= 0.0f) return { 0.0f, 0.0f };

        Vector2f totalLookahead(0.0f, 0.0f);
        float totalWeight = 0.0f;

        for (const auto& t : targets)
        {
            if (std::holds_alternative<Node*>(t.target))
            {
                Node* node = std::get<Node*>(t.target);
                if (node)
                {
                    auto* rb = node->GetComponent<RigidBodyComponent>();
                    if (rb)
                    {
                        totalLookahead += rb->GetVelocity() * lookaheadMultiplier * t.weight;
                        totalWeight += t.weight;
                    }
                }
            }
        }
        return totalWeight > 0.0f ? totalLookahead / totalWeight : totalLookahead;
    }

    Vector2f FollowComponent::CalculateWeightedTargetPosition() const
    {
        if (targets.empty()) return owner->transform->GetPosition();

        Vector2f totalPos(0.0f, 0.0f);
        float totalWeight = 0.0f;

        for (const auto& t : targets)
        {
            Vector2f pos(0, 0);

            if (std::holds_alternative<Node*>(t.target))
            {
                Node* node = std::get<Node*>(t.target);
                if (node && node->transform) pos = node->GetGlobalPosition();
            }
            else
            {
                pos = std::get<Vector2f>(t.target);
            }

            totalPos += pos * t.weight;
            totalWeight += t.weight;
        }

        return totalWeight > 0.0f ? (totalPos / totalWeight) : totalPos;
    }
#pragma endregion

#pragma region Core Loop Helpers
    Vector2f FollowComponent::UpdateLookahead(float fixedDeltaTime)
    {
        Vector2f targetLookahead = CalculateTargetLookahead();
        float tL = 1.0f - std::exp(-lookaheadSmoothing * fixedDeltaTime);
        currentLookahead = currentLookahead + ((targetLookahead - currentLookahead) * tL);
        return currentLookahead;
    }

    void FollowComponent::ProcessDeadzoneAndCentering(const Vector2f& targetPos, float fixedDeltaTime)
    {
        Vector2f diffToRaw = targetPos - virtualFocus;
        Vector2f pushMove(0.0f, 0.0f);

        float halfX = deadzone.x * 0.5f;
        float halfY = deadzone.y * 0.5f;

        bool isOutsideX = std::abs(diffToRaw.x) > halfX;
        bool isOutsideY = std::abs(diffToRaw.y) > halfY;

        // 1. Rigid push if the target exits the box
        if (isOutsideX) pushMove.x = diffToRaw.x - std::copysign(halfX, diffToRaw.x);
        if (isOutsideY) pushMove.y = diffToRaw.y - std::copysign(halfY, diffToRaw.y);

        virtualFocus += pushMove;

        // 2. Anti-jitter control and Timer
        bool isSignificantlyPushing = std::abs(pushMove.x) > 0.5f || std::abs(pushMove.y) > 0.5f;

        if (isSignificantlyPushing)
        {
            if (centeringTimerId != 0)
            {
                Application::Get().GetTimerManager().ClearTimer(centeringTimerId);
                centeringTimerId = 0;
            }
            isWaitingToCenter = false;
            centeringFactor = 0.0f;
        }
        else if (useSoftDeadzone)
        {
            if (!isWaitingToCenter && centeringFactor == 0.0f)
            {
                isWaitingToCenter = true;
                centeringTimerId = Application::Get().GetTimerManager().SetTimeout(centeringDelay, [this]()
                    {
                        this->centeringFactor = 1.0f;
                        this->isWaitingToCenter = false;
                        this->centeringTimerId = 0;
                    });
            }

            if (centeringFactor > 0.0f)
            {
                Vector2f toCenter = targetPos - virtualFocus;
                float d = toCenter.Magnitude();

                if (d > 0.1f)
                {
                    float step = centeringSpeed * fixedDeltaTime;
                    virtualFocus += (toCenter / d) * std::min(step, d);
                }
            }
        }
    }

    void FollowComponent::MoveCamera(const Vector2f& currentCameraPos, float fixedDeltaTime)
    {
        Vector2f camDiff = virtualFocus - currentCameraPos;
        float distSq = camDiff.MagnitudeSquared();
        Vector2f finalCamPos = currentCameraPos;

        // Maximum distance limit forced pull
        if (maxDistance > 0.0f && distSq > (maxDistance * maxDistance))
        {
            float dist = std::sqrt(distSq);
            Vector2f dir = camDiff / dist;
            finalCamPos = virtualFocus - (dir * maxDistance);

            // Recalculate for the switch statement below
            camDiff = virtualFocus - finalCamPos;
            distSq = camDiff.MagnitudeSquared();
        }

        switch (mode)
        {
        case FollowMode::Strict:
            owner->transform->SetPosition(virtualFocus);
            break;
        case FollowMode::Lerp:
        {
            // Framerate-independent Lerp magic formula:
            // position += diff * (1.0 - exp(-speed * deltaTime))
            float tCam = 1.0f - std::exp(-speed * fixedDeltaTime);
            owner->transform->SetPosition(finalCamPos + (camDiff * tCam));
            break;
        }
        case FollowMode::Linear:
        {
            float dist = std::sqrt(distSq);
            float step = speed * fixedDeltaTime;
            if (dist <= step) owner->transform->SetPosition(virtualFocus);
            else owner->transform->SetPosition(finalCamPos + ((camDiff / dist) * step));
            break;
        }
        }
    }
#pragma endregion

    void FollowComponent::FixedUpdate(float fixedDeltaTime)
    {
        if (!owner || !owner->transform || targets.empty()) return;

        Vector2f rawTarget = CalculateWeightedTargetPosition() + offset;
        Vector2f finalTargetPos = rawTarget + UpdateLookahead(fixedDeltaTime);

        if (!initializedFocus)
        {
            virtualFocus = finalTargetPos;
            initializedFocus = true;
        }

        ProcessDeadzoneAndCentering(finalTargetPos, fixedDeltaTime);
        MoveCamera(owner->transform->GetPosition(), fixedDeltaTime);
    }

    void FollowComponent::DebugDraw(IRenderer* renderer)
    {
        if (!renderer || !owner || targets.empty()) return;

        Vector2f rawTarget = CalculateWeightedTargetPosition() + offset;
        Vector2f finalRawTarget = rawTarget + currentLookahead;

        // DRAW BOX ON VIRTUAL FOCUS (Not on Raw Target)
        RectangleShape dzShape{ deadzone };
        Color dzColor = useSoftDeadzone ? Color{ 0, 255, 100, 255 } : Color{ 255, 255, 0, 255 };
        renderer->SubmitDebugShape(RenderLayer::World, dzShape, virtualFocus, 0.0f, dzColor);

        // Targets (Diamonds)
        RectangleShape pointShape{ {10.0f, 10.0f} };
        for (const auto& t : targets)
        {
            Vector2f p = t.GetPosition();
            renderer->SubmitDebugShape(RenderLayer::World, pointShape, p, 45.0f, Color{ 255, 0, 255, 255 });
        }

        // Real center / Final target (Red square)
        RectangleShape centerShape{ {15.0f, 15.0f} };
        renderer->SubmitDebugShape(RenderLayer::World, centerShape, finalRawTarget, 0.0f, Color{ 255, 0, 0, 255 });

        // Current Virtual Focus (Small blue circle) to see exactly what the camera is chasing
        renderer->SubmitDebugShape(RenderLayer::World, RectangleShape{ {5.0f, 5.0f} }, virtualFocus, 0.0f, Color{ 0, 100, 255, 255 });
    }

    std::string FollowComponent::ToString() const
    {
        return "FollowComponent [Targets: " + std::to_string(targets.size()) + "]";
    }
}