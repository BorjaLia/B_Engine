#pragma once

#include <vector>
#include <variant>
#include <string>

#include "Component.h"
#include "../scenes/Node.h"
#include "../utils/Types.h"

namespace Engine
{
    enum class FollowMode
    {
        Strict, // Sticks rigidly to the target (useful for weapons or accessories)
        Lerp,   // Follows smoothly using interpolation (ideal for cameras)
        Linear  // Moves towards the target at a constant speed
    };

    using FollowTargetBase = std::variant<Node*, Vector2f>;

    struct TrackedTarget
    {
        FollowTargetBase target;
        float weight = 1.0f;

        TrackedTarget(Node* n, float w = 1.0f) : target(n), weight(w) {}
        TrackedTarget(Vector2f v, float w = 1.0f) : target(v), weight(w) {}

        Vector2f GetPosition() const
        {
            if (std::holds_alternative<Node*>(target))
            {
                Node* node = std::get<Node*>(target);
                return node ? node->GetGlobalPosition() : Vector2f(0.0f, 0.0f);
            }
            return std::get<Vector2f>(target);
        }
    };

    /// Component that forces the owning node to track or follow a set of targets.
    /// Perfect for dynamic cameras with deadzones and lookahead mechanics.
    /// @ingroup Components
    class FollowComponent : public Component
    {
    public:
        FollowComponent(FollowMode followMode = FollowMode::Lerp, Vector2f speed = { 5.0f,5.0f });
        ~FollowComponent() override = default;

        void FixedUpdate(float fixedDeltaTime) override;
        void DebugDraw(class RendererBase* renderer) override;

        std::string ToString() const override;

        void AddTarget(const TrackedTarget& t) { targets.push_back(t); }
        void SetTargets(const std::vector<TrackedTarget>& newTargets) { targets = newTargets; }
        void ClearTargets() { targets.clear(); }

        void SetMode(FollowMode m) { mode = m; }
        void SetOffset(const Vector2f& off) { offset = off; }
        void SetSpeed(Vector2f& s) { speed = s; }
        void SetMaxDistance(float maxDist) { maxDistance = maxDist; }

        void SetDeadzone(const Vector2f& dz, bool soft = false, float cSpeed = 20.0f)
        {
            deadzone = dz;
            useSoftDeadzone = soft;
            centeringSpeed = cSpeed;
        }

        void SetCenteringDelay(float delay) { centeringDelay = delay; }
        void SetLookahead(float multiplier, float smoothing = 3.0f) { lookaheadMultiplier = multiplier; lookaheadSmoothing = smoothing; }

    private:
        std::vector<TrackedTarget> targets;
        FollowMode mode = FollowMode::Lerp;

        Vector2f offset{ 0.0f, 0.0f };
        Vector2f speed{ 5.0f,5.0f };

        Vector2f virtualFocus{ 0.0f, 0.0f };
        bool initializedFocus = false;

        Vector2f deadzone{ 0.0f, 0.0f };
        bool useSoftDeadzone = false;
        float centeringSpeed = 20.0f;

        float centeringDelay = 1.0f;     // Wait time in seconds
        bool isWaitingToCenter = false;  // Is the timer running?
        float centeringFactor = 0.0f;    // 0 = Waiting/Pushing, 1 = Centering active
        uint32_t centeringTimerId = 0;

        float lookaheadMultiplier = 0.0f;
        float maxDistance = 300.0f;

        Vector2f currentLookahead{ 0.0f, 0.0f };
        float lookaheadSmoothing = 3.0f;

        Vector2f CalculateWeightedTargetPosition() const;
        Vector2f CalculateTargetLookahead() const;

        Vector2f UpdateLookahead(float fixedDeltaTime);
        void ProcessDeadzoneAndCentering(const Vector2f& targetPos, float fixedDeltaTime);
        void MoveCamera(const Vector2f& currentCameraPos, float fixedDeltaTime);
    };
}