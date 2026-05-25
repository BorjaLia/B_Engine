#include "AudioComponent.h"

#include "../scenes/Node.h"
#include "../core/Application.h"
#include "../core/interfaces/IAudio.h"

namespace Engine
{
    AudioComponent::AudioComponent(AudioClip clip, bool playOnStart, bool loop, bool positional, float maxDistance)
        : clip(clip), playOnStart(playOnStart), positional(positional), maxDistance(maxDistance)
    {
        params.loop = loop;
    }

    AudioComponent::~AudioComponent()
    {
        Stop();

        auto& bus = Application::Get().GetEventBus();
        bus.Unsubscribe(AudioPlayEvent::GetStaticType(), playEventId);
        bus.Unsubscribe(AudioStopEvent::GetStaticType(), stopEventId);
        bus.Unsubscribe(AudioMuteEvent::GetStaticType(), muteEventId);
    }

    void AudioComponent::Start()
    {
        auto& bus = Application::Get().GetEventBus();

        // Listen for targeted play requests — only react if the event names this node.
        playEventId = bus.Subscribe<AudioPlayEvent>([this](AudioPlayEvent& e)
            {
                if (e.GetTargetNode() == owner->name)
                {
                    params = e.GetParams();
                    Play();
                }
            });

        stopEventId = bus.Subscribe<AudioStopEvent>([this](AudioStopEvent& e)
            {
                if (e.GetTargetNode() == owner->name)
                    Stop();
            });

        // Global mute events are handled by the IAudio backend directly via Application,
        // but we subscribe here too in case a component wants to react (e.g. show a mute icon).
        muteEventId = bus.Subscribe<AudioMuteEvent>([](AudioMuteEvent&) {});

        if (playOnStart) Play();
    }

    void AudioComponent::Update(float /*deltaTime*/)
    {
        // 1. Check if our logic flags are alive
        if (isPlaying || !params.loop) return;

        IAudio* audio = Application::Get().GetAudio();
        if (!audio) return;

        // Verify if Raylib itself considers the clip playing
        bool rlPlaying = audio->IsPlaying(clip);

        if (!rlPlaying)
        {
            Play();
        }
    }

    void AudioComponent::Play()
    {
        IAudio* audio = Application::Get().GetAudio();
        if (!audio || clip.id == 0) return;

        if (positional)
        {
            // Find the listener position. If none exists, fall back to global playback.
            Vector2f listenerPos = owner->transform.GetGlobalPosition();

            Node* listenerNode = Application::Get().FindAudioListener();
            if (listenerNode) listenerPos = listenerNode->transform.GetGlobalPosition();

            audio->PlayPositional(clip, params, listenerPos, owner->transform.GetGlobalPosition(), maxDistance);
        }
        else
        {
            audio->Play(clip, params);
        }
    }

    void AudioComponent::Stop()
    {
        IAudio* audio = Application::Get().GetAudio();
        if (audio) audio->Stop(clip);
    }
}