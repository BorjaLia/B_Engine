#pragma once

#include <string>
#include "Event.h"
#include "../utils/Types.h"

namespace Engine
{
    /// Fired to request that a specific AudioComponent plays its clip.
    /// The target is identified by the name of its owner node.
    /// @ingroup Events
    class AudioPlayEvent : public EventBase<AudioPlayEvent>
    {
    public:
        AudioPlayEvent(std::string targetNode, AudioPlayParams params = {})
            : targetNode(std::move(targetNode)), params(params)
        {
        }

        const std::string& GetTargetNode() const { return targetNode; }
        const AudioPlayParams& GetParams()   const { return params; }

        const char* GetName() const override { return "AudioPlayEvent"; }

    private:
        std::string     targetNode;
        AudioPlayParams params;
    };

    /// Fired to request that a specific AudioComponent stops playback.
    /// @ingroup Events
    class AudioStopEvent : public EventBase<AudioStopEvent>
    {
    public:
        AudioStopEvent(std::string targetNode) : targetNode(std::move(targetNode)) {}

        const std::string& GetTargetNode() const { return targetNode; }
        const char* GetName() const override { return "AudioStopEvent"; }

    private:
        std::string targetNode;
    };

    /// Fired to globally mute or unmute all audio outputs.
    /// @ingroup Events
    class AudioMuteEvent : public EventBase<AudioMuteEvent>
    {
    public:
        AudioMuteEvent(bool mute) : mute(mute) {}

        bool GetMute() const { return mute; }
        const char* GetName() const override { return "AudioMuteEvent"; }

    private:
        bool mute;
    };

    /// Fired to change the master volume globally.
    /// @ingroup Events
    class AudioVolumeEvent : public EventBase<AudioVolumeEvent>
    {
    public:
        AudioVolumeEvent(float volume) : volume(volume) {}

        float GetVolume() const { return volume; }
        const char* GetName() const override { return "AudioVolumeEvent"; }

    private:
        float volume;
    };
}