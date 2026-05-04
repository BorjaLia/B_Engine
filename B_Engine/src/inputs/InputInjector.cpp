#include "InputInjector.h"

#include <cstring>
#include <sstream>

#include "../core/Application.h"
#include "../debug/Debug.h"
#include "../utils/FileSystem.h"
#include "../utils/StringHash.h"
#include "../time/Time.h"

namespace Engine
{
	bool InputInjector::LoadFromFile(const std::string& filename)
	{
		std::vector<uint8_t> buffer;
		if (!FileSystem::ReadBinary(filename, buffer))
		{
			ENGINE_ERROR("InputInjector: Couldn't open file: {}", filename);
			return false;
		}

		size_t offset = 0;

		// 1. Read metadata size
		size_t metaSize = 0;
		std::memcpy(&metaSize, buffer.data() + offset, sizeof(size_t));
		offset += sizeof(size_t);

		// 2. Extract metadata string (e.g. RNG State)
		std::string metadata;
		if (metaSize > 0)
		{
			metadata.resize(metaSize);
			std::memcpy(metadata.data(), buffer.data() + offset, metaSize);
			offset += metaSize;

			size_t pipePos = metadata.find('|');
			if (pipePos != std::string::npos)
			{
				std::string resStr = metadata.substr(4, pipePos - 4);
				std::string devMeta = metadata.substr(pipePos + 1);

				int w = 1920, h = 1080; char x;
				std::stringstream ss(resStr);
				ss >> w >> x >> h;

				Application::Get().GetRenderer()->SetLogicalResolution({ w, h });

				metadata = devMeta;
			}
		}

		// 3. Read event count
		size_t count = 0;
		std::memcpy(&count, buffer.data() + offset, sizeof(size_t));
		offset += sizeof(size_t);

		// 4. Extract the actual payload (Replay Entries)
		playbackData.resize(count);
		std::memcpy(playbackData.data(), buffer.data() + offset, count * sizeof(ReplayEntry));

		ENGINE_INFO("InputInjector: Replay loaded with {} events.", count);

		if (!metadata.empty())
		{
			if (metadataConsumer)
			{
				// Game is running, inject immediately
				metadataConsumer(metadata);
				ENGINE_INFO("InputInjector: Metadata injected.");
			}
			else
			{
				// Game hasn't fully started yet (AutoPlayback). Keep in pocket.
				pendingMetadata = metadata;
				ENGINE_INFO("InputInjector: Metadata saved in cache. Waiting for game...");
			}
		}

		return true;
	}

	void InputInjector::Play(bool reset)
	{
		if (playbackData.empty()) return;

		isPlaying = true;
		ReplayStateEvent e(true);
		Application::Get().GetEventBus().Publish(e);
		currentPlaybackIndex = 0;

		if (reset) Time::ResetTicks();

		ENGINE_INFO("InputInjector: replay started...");
	}

	void InputInjector::Stop()
	{
		isPlaying = false;
		ReplayStateEvent e(false);
		Application::Get().GetEventBus().Publish(e);

		Vector2i realWindowSize = Application::Get().GetWindow()->GetSize();
		Application::Get().GetRenderer()->SetLogicalResolution(realWindowSize);
		ENGINE_INFO("InputInjector: replay stopped...");

#ifdef _DEBUG

		int fps = Time::GetFrames();
		LogLevel level = Engine::Logger::GetLevel();

		Engine::Logger::SetLevel(Engine::LogLevel::All);
		ENGINE_LOG("---REPLAY DATA---");
		ENGINE_LOG("Replay had {} frames in {} ticks ", fps, Time::GetTicks());
		ENGINE_LOG("Time to complete: {} ", Time::GetTime());
		ENGINE_LOG("Average fps: {} ", (static_cast<float>(fps) / Time::GetTime()));
		ENGINE_LOG("-----------------");
		Engine::Logger::SetLevel(level);
#endif
	}

	void InputInjector::Update()
	{
		if (!isPlaying || currentPlaybackIndex >= playbackData.size())
		{
			if (isPlaying) Stop(); // Reached the end of the tape
			return;
		}

		int currentEngineTick = Time::GetTicks();
		auto& mapper = Application::Get().GetInputManager();

		// While there are events belonging to THIS tick or past ones
		while (currentPlaybackIndex < playbackData.size() &&
			playbackData[currentPlaybackIndex].tick <= currentEngineTick)
		{
			const ReplayEntry& entry = playbackData[currentPlaybackIndex];

			uint32_t hashX = Hash::GetHash("Pointer_X");
			uint32_t hashY = Hash::GetHash("Pointer_Y");

			if (entry.actionHash == hashX || entry.actionHash == hashY)
			{
				ENGINE_INPUT_MOUSE("Injecting -> Tick: {} | Hash: {} | Val: {}",
					entry.tick, entry.actionHash, entry.value);
			}
			else
			{
				ENGINE_INFO("Injecting -> Tick: {} | Hash: {} | Val: {}",
					entry.tick, entry.actionHash, entry.value);
			}

			// DIRECT INJECTION: 
			// Bypass hardware by forcefully overriding the mapped absolute state.
			mapper.SetAbsoluteAxis(entry.actionHash, entry.value);

			currentPlaybackIndex++;
		}
	}

	void InputInjector::SetMetadataConsumer(std::function<void(const std::string&)> consumer)
	{
		metadataConsumer = consumer;

		// If we held onto metadata waiting for this consumer to exist... deliver it!
		if (!pendingMetadata.empty())
		{
			metadataConsumer(pendingMetadata);
			ENGINE_INFO("InputInjector: Pending metadata injected successfully.");
			pendingMetadata.clear();
		}
	}

	void InputInjector::DumpReplaySummary() const
	{
#ifdef _DEBUG
		if (playbackData.empty()) return;

		ENGINE_INFO("--- LOADED REPLAY SUMMARY (F7) ---");
		for (const auto& entry : playbackData)
		{
			ENGINE_INFO("Tick: {} | Hash: {} | Val: {}", entry.tick, entry.actionHash, entry.value);
		}
#endif
	}
}