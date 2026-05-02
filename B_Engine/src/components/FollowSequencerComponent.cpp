#include "FollowSequencerComponent.h"

#include "../scenes/Node.h"
#include "../debug/Debug.h"

namespace Engine
{
    void FollowSequencerComponent::Start()
    {
        if (owner)
        {
            followComp = owner->GetComponent<FollowComponent>();

            if (!followComp)
            {
                ENGINE_WARN("FollowSequencer needs a FollowComponent attached to the same node!");
            }
        }
    }

    void FollowSequencerComponent::Play()
    {
        if (sequence.empty() || !followComp) return;

        isPlaying = true;

        followComp->SetTargets(sequence[currentIndex].targets);
        timer = sequence[currentIndex].duration;
    }

    void FollowSequencerComponent::Pause()
    {
        isPlaying = false;
    }

    void FollowSequencerComponent::Stop()
    {
        isPlaying = false;
        currentIndex = 0;
        timer = 0.0f;
    }

    void FollowSequencerComponent::Update(float deltaTime)
    {
        if (!isPlaying || !followComp || sequence.empty()) return;

        timer -= deltaTime;

        if (timer <= 0.0f)
        {
            currentIndex++;

            if (currentIndex >= sequence.size())
            {
                if (loop)
                {
                    currentIndex = 0;
                }
                else
                {
                    Stop();
                    return;
                }
            }

            followComp->SetTargets(sequence[currentIndex].targets);
            timer = sequence[currentIndex].duration + timer;
        }
    }
}