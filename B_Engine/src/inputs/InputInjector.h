#pragma once

#include <vector>
#include <functional>
#include <string>

#include "InputLogger.h"

namespace Engine
{
    /// Reads recorded input logs and physically injects them back into the InputMapper.
    /// @ingroup Inputs
    class InputInjector
    {
    public:
        InputInjector() = default;

        /// Loads a binary replay file previously saved by the InputLogger.
        bool LoadFromFile(const std::string& filename);

        /// Starts playback of the loaded replay.
        /// @param reset Resets the engine ticks back to 0 if true.
        void Play(bool reset = false);

        /// Stops playback and returns control to the player.
        void Stop();

        /// Feeds the mapped actions for the current engine tick.
        void Update();

        bool IsPlaying() const { return isPlaying; }

        const std::vector<ReplayEntry>& GetPlaybackData() const { return playbackData; }

        /// Subscribes a callback to receive metadata contained within a replay file.
        void SetMetadataConsumer(std::function<void(const std::string&)> consumer);

        /// Dumps the loaded events to the console (Debug only)
        void DumpReplaySummary() const;

    private:
        std::vector<ReplayEntry> playbackData;
        size_t currentPlaybackIndex = 0;

        std::function<void(const std::string&)> metadataConsumer;
        std::string pendingMetadata;

        bool isPlaying = false;
    };
}