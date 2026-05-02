#pragma once

#include "Component.h"

namespace Engine
{
    /// Marks a node as the "ear" of the world for positional audio.
    /// Only one listener should be active at a time (typically on the camera or player node).
    /// AudioComponents in positional mode query the scene for this component upon each Play() call.
    class AudioListenerComponent : public Component
    {
    public:
        AudioListenerComponent() = default;
        ~AudioListenerComponent() override = default;
    };
}