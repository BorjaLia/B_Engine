#pragma once

#include <unordered_map>
#include "../../core/interfaces/IAudio.h"
#include "../../utils/Types.h"

namespace Engine
{
    /// Raylib-backed audio implementation.
    /// Uses opaque pointers to ensure Raylib headers do not leak into the engine core.
    class RaylibAudio : public IAudio
    {
    public:
        bool Initialize() override;
        void Shutdown() override;

        AudioClip LoadClip(const char* filepath) override;
        void UnloadClip(AudioClip clip) override;

        void Play(AudioClip clip, const AudioPlayParams& params = {}) override;
        void Stop(AudioClip clip) override;
        bool IsPlaying(AudioClip clip) override;

        void SetMasterMuted(bool muted) override;
        void SetMasterVolume(float volume) override;

        void PlayPositional(AudioClip clip, const AudioPlayParams& params,
            const Vector2f& listenerPos,
            const Vector2f& sourcePos,
            float maxDistance) override;

    private:
        // Maps our opaque AudioClip ID to the raw Raylib Sound struct.
        // We use void* (Opaque Pointer pattern) to avoid including raylib.h here.
        struct SoundEntry { void* internalData = nullptr; };

        std::unordered_map<unsigned int, SoundEntry> loadedSounds;
        unsigned int nextId = 1;

        bool masterMuted = false;
        float masterVolume = 1.0f;
    };
}