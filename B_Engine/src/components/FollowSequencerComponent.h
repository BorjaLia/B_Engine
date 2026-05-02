#pragma once

#include <vector>

#include "Component.h"
#include "FollowComponent.h"

namespace Engine
{
    struct FollowKeyframe
    {
        std::vector<TrackedTarget> targets;
        float duration = 1.0f;

        // Optional for future expansion:
        // float newSpeed = 5.0f; 
        // FollowMode newMode = FollowMode::Lerp;
    };

    /// Orchestrates a sequence of targets for a FollowComponent over time.
    /// Ideal for cinematic camera sweeps or boss intros.
    /// @ingroup Components
    class FollowSequencerComponent : public Component
    {
    public:
        FollowSequencerComponent() = default;
        ~FollowSequencerComponent() override = default;

        void Start() override;
        void Update(float deltaTime) override;

        void AddKeyframe(const FollowKeyframe& keyframe) { sequence.push_back(keyframe); }
        void SetLooping(bool shouldLoop) { loop = shouldLoop; }

        void Play();
        void Pause();
        void Stop();

        bool IsPlaying() const { return isPlaying; }

        void ClearSequence()
        {
            sequence.clear();
            currentIndex = 0;
            timer = 0.0f;
        }

    private:
        FollowComponent* followComp = nullptr;

        std::vector<FollowKeyframe> sequence;
        int currentIndex = 0;
        float timer = 0.0f;

        bool isPlaying = false;
        bool loop = false;
    };
}