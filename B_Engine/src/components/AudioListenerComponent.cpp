#include "AudioListenerComponent.h"

#include "../core/Application.h"

namespace Engine
{
    AudioListenerComponent::~AudioListenerComponent()
    {
        Application::Get().UnregisterAudioListener(this);
    }

    void AudioListenerComponent::Start()
    {
        Application::Get().RegisterAudioListener(this);
    }
}