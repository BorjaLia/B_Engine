#include "raylibAudio.h"

#include <raylib.h>
#include <cmath>
#include <algorithm>

namespace Engine
{
    bool RaylibAudio::Initialize()
    {
        ::InitAudioDevice();
        return ::IsAudioDeviceReady();
    }

    void RaylibAudio::Shutdown()
    {
        // Unload all sounds still in the map
        for (auto& [id, entry] : loadedSounds)
        {
            ::Sound* s = static_cast<::Sound*>(entry.internalData);
            ::UnloadSound(*s);
            delete s;
        }
        loadedSounds.clear();
        ::CloseAudioDevice();
    }

    AudioClip RaylibAudio::LoadClip(const char* filepath)
    {
        // 1. Creamos el struct completo en el Heap
        ::Sound* rlSound = new ::Sound;
        *rlSound = ::LoadSound(filepath);

        if (rlSound->frameCount == 0)
        {
            delete rlSound; // Limpiamos si falló
            return AudioClip{};
        }
        unsigned int ourId = nextId++;
        // We re-identify Raylib's Sound by casting its internal buffer pointer to an integer.
        // This avoids storing the full Raylib Sound struct (which would require the header elsewhere).
        loadedSounds[ourId] = { rlSound };


        AudioClip clip;
        clip.id = ourId;
        return clip;
    }

    void RaylibAudio::UnloadClip(AudioClip clip)
    {
        auto it = loadedSounds.find(clip.id);
        if (it == loadedSounds.end()) return;

        // 3. Recuperamos el puntero, lo descargamos de la GPU/RAM de audio y lo borramos
        ::Sound* s = static_cast<::Sound*>(it->second.internalData);
        ::UnloadSound(*s);
        delete s;

        loadedSounds.erase(it);
    }

    void RaylibAudio::Play(AudioClip clip, const AudioPlayParams& params)
    {
        auto it = loadedSounds.find(clip.id);
        if (it == loadedSounds.end()) return;

        ::Sound* s = static_cast<::Sound*>(it->second.internalData);
        float effectiveVolume = masterMuted ? 0.0f : params.volume * masterVolume;

        ::SetSoundVolume(*s, effectiveVolume);
        ::SetSoundPitch(*s, params.pitch);
        ::PlaySound(*s);
    }

    void RaylibAudio::Stop(AudioClip clip)
    {
        auto it = loadedSounds.find(clip.id);
        if (it == loadedSounds.end()) return;

        ::Sound* s = static_cast<::Sound*>(it->second.internalData);
        ::StopSound(*s);
    }

    bool RaylibAudio::IsPlaying(AudioClip clip)
    {
        auto it = loadedSounds.find(clip.id);
        if (it == loadedSounds.end()) return false;

        ::Sound* s = static_cast<::Sound*>(it->second.internalData);
        return ::IsSoundPlaying(*s);
    }

    void RaylibAudio::SetMasterMuted(bool muted)
    {
        masterMuted = muted;
        ::SetMasterVolume(muted ? 0.0f : masterVolume);
    }

    void RaylibAudio::SetMasterVolume(float volume)
    {
        masterVolume = std::clamp(volume, 0.0f, 1.0f);
        if (!masterMuted) ::SetMasterVolume(masterVolume);
    }

    void RaylibAudio::PlayPositional(AudioClip clip, const AudioPlayParams& params,
                                     const Vector2f& listenerPos,
                                     const Vector2f& sourcePos,
                                     float           maxDistance)
    {
        if (maxDistance <= 0.0f) return;

        // Distance attenuation: linear falloff from 1.0 at distance=0 to 0.0 at maxDistance.
        // This is the simplest model — good enough for 2D. More sophisticated models
        // (inverse square, logarithmic) can be swapped in here later without touching callers.
        float dx = sourcePos.x - listenerPos.x;
        float dy = sourcePos.y - listenerPos.y;
        float dist = std::sqrt(dx * dx + dy * dy);
        float t = std::clamp(1.0f - dist / maxDistance, 0.0f, 1.0f);
        
        if (t <= 0.0f) return; // Too far to hear

        AudioPlayParams attenuated = params;
        attenuated.volume = params.volume * t;
        Play(clip, attenuated);
    }
}