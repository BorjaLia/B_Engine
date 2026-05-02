#pragma once

#include <string>

#include "../debug/Debug.h"
#include "ReplayModes.h"

namespace Engine
{
#pragma region Data Structs
    struct EngineConfig
    {
        std::string backend = "OpenGL";
        LogLevel logLevel = LogLevel::All;
        EngineMode replayMode = EngineMode::Normal;
        float fixedTimeStep = 0.01666f;
        int targetFPS = 60;
        std::string resourceDirectory = "./assets/";
        bool enableDevTools = true;
    };

    struct UserSettings
    {
        int windowWidth = 1920;
        int windowHeight = 1080;
        bool fullscreen = true;
        bool vSync = false;
        bool showFPS = false;

        float masterVolume = 1.0f;
        float musicVolume = 1.0f;
        float sfxVolume = 1.0f;

        std::string language = "en";
        int shadowQuality = 2;
        int antiAliasing = 4;
    };
#pragma endregion

    /// Manages loading and saving configuration files (.ini) for both engine and user settings.
    class ConfigManager
    {
    public:
        /// Loads engine and user configurations from disk.
        /// Expected to be called once during Application initialization.
        static void LoadAll();

        /// Retrieves the current read-only engine configuration.
        static const EngineConfig& GetEngineConfig() { return engineData; }

        /// Retrieves the current read-only user settings.
        static const UserSettings& GetUserSettings() { return userData; }

        /// Applies new user settings and serializes them to user_settings.ini.
        /// @param newSettings The new UserSettings struct to apply.
        static void ApplyAndSaveUserSettings(const UserSettings& newSettings);

        /// Applies new engine configurations and serializes them to engine.ini.
        /// @param newConfig The new EngineConfig struct to apply.
        static void ApplyAndSaveEngineConfig(const EngineConfig& newConfig);

    private:
        static EngineConfig engineData;
        static UserSettings userData;

        static void ParseEngineLine(const std::string& key, const std::string& value);
        static void ParseUserLine(const std::string& key, const std::string& value);
        static std::string Trim(const std::string& str);

        static LogLevel StringToLogLevel(const std::string& str);
        static std::string LogLevelToString(LogLevel level);
        static EngineMode StringToReplayMode(const std::string& str);
        static std::string ReplayModeToString(EngineMode mode);
    };
}