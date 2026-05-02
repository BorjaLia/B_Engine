#pragma once

#include <string>
#include "../../utils/Types.h"

namespace Engine
{
    /// Pure interface for the audio backend.
    /// Mirrors the pattern used by IRenderer: one interface, one backend implementation.
    class IAudio
    {
    public:
        virtual ~IAudio() = default;

        /// Initializes the audio device. Called once at startup.
        virtual bool Initialize() = 0;

        /// Releases the audio device and all loaded sounds.
        virtual void Shutdown() = 0;

        /// Loads a sound file from disk and returns an opaque handle.
        /// @param filepath Path to the audio file.
        virtual AudioClip LoadClip(const char* filepath) = 0;

        /// Releases a previously loaded clip from memory.
        virtual void UnloadClip(AudioClip clip) = 0;

        /// Plays a sound with the given parameters. Returns immediately.
        virtual void Play(AudioClip clip, const AudioPlayParams& params = {}) = 0;

        /// Stops all currently playing instances of this clip.
        virtual void Stop(AudioClip clip) = 0;

        /// Checks if a specific audio clip is currently playing.
        virtual bool IsPlaying(AudioClip clip) = 0;

        /// Pauses or resumes all audio globally.
        virtual void SetMasterMuted(bool muted) = 0;

        /// Sets the master volume.
        /// @param volume Float multiplier between 0.0 and 1.0.
        virtual void SetMasterVolume(float volume) = 0;

        /// Plays a sound attenuated by world-space distance.
        /// @param listenerPos Position of the AudioListenerComponent.
        /// @param sourcePos Position of the AudioComponent in the world.
        /// @param maxDistance Distance at which the sound becomes inaudible.
        virtual void PlayPositional(AudioClip clip, const AudioPlayParams& params, const Vector2f& listenerPos, const Vector2f& sourcePos, float maxDistance) = 0;
    };
}