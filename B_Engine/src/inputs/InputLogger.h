#pragma once

#include <functional>
#include <vector>
#include <string>
#include <cstdint>

#include "../events/EventBus.h"
#include "../events/InputEvents.h"

namespace Engine
{
    /// Minimal struct for a replay frame event (Only 12 bytes!)
    /// @ingroup Inputs
    struct ReplayEntry
    {
        int tick;
        uint32_t actionHash;
        float value;
    };

    /// Captures input events and stores them sequentially for later replay or debugging.
    /// @ingroup Inputs
    class InputLogger
    {
    public:
        InputLogger() = default;
        ~InputLogger();

        void Initialize();

        /// Starts recording input actions.
        /// @param includeSystem Set to true to record debug/system inputs as well.
        void StartRecording(bool includeSystem = false);
        void StopRecording();

        void OnActionChanged(ActionChangedEvent& e);

        const std::vector<ReplayEntry>& GetRecordedData() const { return recordedEvents; }
        void Clear() { recordedEvents.clear(); }

        /// Dumps the current recorded input buffer to a binary file.
        void SaveToFile(const std::string& filename);

        bool IsRecording() const { return isRecording; }

        /// Injects a custom string payload (e.g. RNG seed) into the replay file header.
        void SetMetadataProvider(std::function<std::string()> provider) { metadataProvider = provider; }

    private:
        std::vector<ReplayEntry> recordedEvents;
        uint32_t actionSubId = 0;

        std::function<std::string()> metadataProvider;

        bool isRecording = false;
        bool logSystemActions = false;
    };
}