#include "InputLogger.h"

#include <cstring> 

#include "../core/Application.h"
#include "../debug/Debug.h"
#include "../utils/FileSystem.h" 
#include "../utils/StringHash.h"

namespace Engine
{
    InputLogger::~InputLogger()
    {
        Application::Get().GetEventBus().Unsubscribe(ActionChangedEvent::GetStaticType(), actionSubId);
    }

    void InputLogger::Initialize()
    {
        actionSubId = Application::Get().GetEventBus().Subscribe<ActionChangedEvent>(
            [this](ActionChangedEvent& e) { this->OnActionChanged(e); }
        );
    }

    void InputLogger::StartRecording(bool includeSystem)
    {
        recordedEvents.clear();
        isRecording = true;
        logSystemActions = includeSystem;

        ENGINE_INFO("InputLogger: recording started (Mode: {})", includeSystem ? "QA/System" : "Gameplay Only");
    }

    void InputLogger::StopRecording()
    {
        isRecording = false;
        ENGINE_INFO("InputLogger: recording finalized. Captured events: {}", recordedEvents.size());
    }

    void InputLogger::OnActionChanged(ActionChangedEvent& e)
    {
        if (!isRecording) return;

        if (e.GetCategory() == ActionCategory::System && !logSystemActions) return;

        recordedEvents.push_back({ e.GetTick(), e.GetHash(), e.GetValue() });

        // VISUAL LOG FILTERING
        uint32_t hashX = Hash::GetHash("Pointer_X");
        uint32_t hashY = Hash::GetHash("Pointer_Y");

        if (e.GetHash() == hashX || e.GetHash() == hashY)
        {
            ENGINE_INPUT_MOUSE("Recording -> Tick: {} | Hash: {} | Val: {} ", e.GetTick(), e.GetHash(), e.GetValue());
        }
        else
        {
            ENGINE_INPUT("Recording -> Tick: {} | Hash: {} | Val: {} ", e.GetTick(), e.GetHash(), e.GetValue());
        }
    }

    void InputLogger::SaveToFile(const std::string& filename)
    {
        if (recordedEvents.empty()) return;

        Vector2i res = Application::Get().GetRenderer()->GetLogicalResolution();
        std::string devMeta = metadataProvider ? metadataProvider() : "";

        // 1. Check if the game provided any custom metadata (like an RNG seed)
        std::string metadata = std::format("RES:{}x{}|{}", res.x, res.y, devMeta);
        size_t metaSize = metadata.size();

        size_t count = recordedEvents.size();

        // Total File Size: [Meta Size (8)] + [Meta Text (X)] + [Event Count (8)] + [Events (Y)]
        size_t totalBytes = sizeof(size_t) + metaSize + sizeof(size_t) + (count * sizeof(ReplayEntry));

        std::vector<uint8_t> buffer(totalBytes);
        size_t offset = 0;

        // Write metadata string size
        std::memcpy(buffer.data() + offset, &metaSize, sizeof(size_t));
        offset += sizeof(size_t);

        // Write the metadata string (if any)
        if (metaSize > 0)
        {
            std::memcpy(buffer.data() + offset, metadata.data(), metaSize);
            offset += metaSize;
        }

        // Write event count
        std::memcpy(buffer.data() + offset, &count, sizeof(size_t));
        offset += sizeof(size_t);

        // Write the event data array
        std::memcpy(buffer.data() + offset, recordedEvents.data(), count * sizeof(ReplayEntry));

        if (FileSystem::WriteBinary(filename, buffer.data(), totalBytes))
        {
            ENGINE_INFO("InputLogger: Replay saved at {} ({} events)", filename, count);
        }
        else
        {
            ENGINE_ERROR("InputLogger: Error while saving replay: {}", filename);
        }
    }

    void InputLogger::DumpReplaySummary() const
    {
#ifdef _DEBUG
        if (recordedEvents.empty()) return;

        ENGINE_INFO("--- SAVED REPLAY SUMMARY (F6) ---");
        for (const auto& entry : recordedEvents)
        {
            ENGINE_INFO("Tick: {} | Hash: {} | Val: {}", entry.tick, entry.actionHash, entry.value);
        }
#endif
    }
}