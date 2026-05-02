#pragma once

#include <string>
#include "Event.h"

namespace Engine
{
    class Node; // Forward declaration to avoid including Node.h here

    /// Triggered when an animated sprite component finishes its current animation cycle.
    /// @ingroup Events
    class AnimationFinishedEvent : public EventBase<AnimationFinishedEvent>
    {
    public:
        AnimationFinishedEvent(const std::string& name, Node* owner)
            : animationName(name), ownerNode(owner)
        {
        }

        std::string GetAnimationName() const { return animationName; }
        Node* GetNode() const { return ownerNode; }

        const char* GetName() const override { return "AnimationFinishedEvent"; }

    private:
        std::string animationName;
        Node* ownerNode;
    };
}