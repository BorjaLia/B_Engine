#pragma once

#include <string>

#include "../debug/Debug.h"
#include "ReplayModes.h"

namespace Engine
{
#pragma region Data Structs

    /// Configuration data for the engine's core systems and initialization.
    struct EngineConfig
    {
        /// Graphics backend to be used by the engine (e.g. Raylib).
        std::string backend = "Raylib";

        /// Specifies the verbosity of the debug console.
        LogLevel logLevel = LogLevel::All;

        /// Determines the current playback or recording state of the engine.
        EngineMode replayMode = EngineMode::Normal;

        /// The fixed delta time used for physics calculations (e.g., 0.01666f for 60Hz).
        float fixedTimeStep = 0.01666f;

        /// The root directory where game resources (textures, sounds, etc.) are located.
        std::string resourceDirectory = "./res/";

        /// Toggles the availability of developer tools and debug overlays.
        bool enableDevTools = true;

        /// If true, forces the screen to maintain a specific aspect ratio.
        bool lockAspectRatio = true;
    };

    /// Configuration data representing user-specific game preferences.
    struct UserSettings
    {
        /// The width of the game window or screen resolution in pixels.
        int windowWidth = 1920;

        /// The height of the game window or screen resolution in pixels.
        int windowHeight = 1080;

        /// Toggles full-screen mode on or off.
        bool fullscreen = false;

        /// Toggles vertical synchronization to prevent screen tearing.
        bool vSync = false;

        /// The maximum frames per second allowed. Set to 0 for unlocked framerate.
        int targetFPS = 60;

        /// Toggles the frames-per-second counter overlay.
        bool showFPS = false;

        /// Overall brightness/gamma correction (1.0 is default).
        float brightness = 1.0f;

        /// Scaling multiplier for the UI elements (1.0 is default).
        float uiScale = 1.0f;

        /// The master volume multiplier (0.0 to 1.0).
        float masterVolume = 1.0f;

        /// The music volume multiplier (0.0 to 1.0).
        float musicVolume = 1.0f;

        /// The sound effects volume multiplier (0.0 to 1.0).
        float sfxVolume = 1.0f;

        /// The currently selected language code (e.g., "en", "es").
        std::string language = "en";

        /// The shadow rendering quality (e.g., 0 = Low, 1 = Medium, 2 = High).
        int shadowQuality = 2;

        /// The level of anti-aliasing applied (e.g., 0, 2, 4, 8).
        int antiAliasing = 4;
    };

#pragma endregion

    /// Manages loading and saving configuration files (.ini) for both engine and user settings.
    /// @ingroup Core
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