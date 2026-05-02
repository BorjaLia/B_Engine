#pragma once

#include "Component.h"
#include "../utils/Types.h"
#include "../events/AudioEvents.h"

namespace Engine
{
    /// Plays a sound either globally or attenuated by distance from an AudioListenerComponent.
    /// Subscribe to AudioPlayEvent / AudioStopEvent to trigger playback via the event bus.
    class AudioComponent : public Component
    {
    public:
        /// @param clip         The sound to play.
        /// @param playOnStart  If true, the clip plays automatically when the scene starts.
        /// @param loop         If true, the clip restarts automatically upon completion.
        /// @param positional   If true, volume is attenuated by distance to the listener.
        /// @param maxDistance  World-space distance at which the sound becomes inaudible (positional only).
        AudioComponent(AudioClip clip,
            bool playOnStart = false,
            bool loop = false,
            bool positional = false,
            float maxDistance = 500.0f);

        ~AudioComponent() override;

        void Start() override;
        void Update(float deltaTime) override;

        /// Plays the clip with the component's current parameters.
        void Play();

        /// Stops the clip.
        void Stop();

        void SetClip(AudioClip newClip) { clip = newClip; }
        void SetParams(AudioPlayParams p) { params = p; }

        void SetLoop(bool l) { params.loop = l; }
        bool GetLoop() const { return params.loop; }

        void SetPositional(bool p) { positional = p; }
        void SetMaxDistance(float d) { maxDistance = d; }

        AudioClip GetClip() const { return clip; }
        AudioPlayParams GetParams() const { return params; }
        bool IsPositional() const { return positional; }
        float GetMaxDistance() const { return maxDistance; }

    private:
        AudioClip clip;
        AudioPlayParams params;

        bool playOnStart = false;
        bool positional = false;
        float maxDistance = 500.0f;

        bool isPlaying = false;

        uint32_t playEventId = 0;
        uint32_t stopEventId = 0;
        uint32_t muteEventId = 0;
    };
}