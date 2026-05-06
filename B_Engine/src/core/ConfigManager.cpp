#include "ConfigManager.h"

#include "Application.h"
#include "../debug/Debug.h"
#include "../events/ApplicationEvents.h"
#include "../utils/FileSystem.h"

#include <sstream>
#include <algorithm>

namespace Engine
{
    EngineConfig ConfigManager::engineData;
    UserSettings ConfigManager::userData;

    LogLevel ConfigManager::StringToLogLevel(const std::string& str)
    {
        if (str == "Error") return LogLevel::Error;
        if (str == "Warn")  return LogLevel::Warn;
        if (str == "Info")  return LogLevel::Info;
        if (str == "None")  return LogLevel::None;
        if (str == "NoInput")  return LogLevel::NoInput;
        if (str == "NoMouseInput")  return LogLevel::NoMouseInput;
        return LogLevel::All;
    }

    std::string ConfigManager::LogLevelToString(LogLevel level)
    {
        if (level == LogLevel::Error) return "Error";
        if (level == LogLevel::Warn)  return "Warn";
        if (level == LogLevel::Info)  return "Info";
        if (level == LogLevel::None)  return "None";
        if (level == LogLevel::NoInput)  return "NoInput";
        if (level == LogLevel::NoMouseInput)  return "NoMouseInput";
        return "All";
    }

    EngineMode ConfigManager::StringToReplayMode(const std::string& str)
    {
        if (str == "AutoRecord")   return EngineMode::AutoRecord;
        if (str == "AutoPlayback") return EngineMode::AutoPlayback;
        return EngineMode::Normal;
    }

    std::string ConfigManager::ReplayModeToString(EngineMode mode)
    {
        if (mode == EngineMode::AutoRecord)   return "AutoRecord";
        if (mode == EngineMode::AutoPlayback) return "AutoPlayback";
        return "Normal";
    }

    std::string ConfigManager::Trim(const std::string& str)
    {
        size_t first = str.find_first_not_of(' ');
        if (std::string::npos == first) return "";
        size_t last = str.find_last_not_of(' ');
        return str.substr(first, (last - first + 1));
    }

    void ConfigManager::LoadAll()
    {
        std::string engineText;
        if (FileSystem::ReadText("engine.ini", engineText))
        {
            std::istringstream stream(engineText);
            std::string line;
            while (std::getline(stream, line))
            {
                line = Trim(line);
                if (line.empty() || line[0] == '#') continue;

                size_t equalsPos = line.find('=');
                if (equalsPos != std::string::npos)
                {
                    std::string key = Trim(line.substr(0, equalsPos));
                    std::string value = Trim(line.substr(equalsPos + 1));
                    ParseEngineLine(key, value);
                }
            }
        }
        else
        {
            ENGINE_WARN("Couldn't find engine.ini. Generating default configuration...");
            ApplyAndSaveEngineConfig(engineData);
        }

        std::string userText;
        if (FileSystem::ReadText("user_settings.ini", userText))
        {
            std::istringstream stream(userText);
            std::string line;
            while (std::getline(stream, line))
            {
                line = Trim(line);
                if (line.empty() || line[0] == '#') continue;

                size_t equalsPos = line.find('=');
                if (equalsPos != std::string::npos)
                {
                    std::string key = Trim(line.substr(0, equalsPos));
                    std::string value = Trim(line.substr(equalsPos + 1));
                    ParseUserLine(key, value);
                }
            }
        }
        else
        {
            ENGINE_WARN("Couldn't find user_settings.ini. Generating default settings...");
            ApplyAndSaveUserSettings(userData);
        }
    }

    void ConfigManager::ParseEngineLine(const std::string& key, const std::string& value)
    {
        if (key == "Backend") engineData.backend = value;
        else if (key == "LogLevel") engineData.logLevel = StringToLogLevel(value);
        else if (key == "ReplayMode") engineData.replayMode = StringToReplayMode(value);
        else if (key == "FixedTimeStep") engineData.fixedTimeStep = std::stof(value);
        else if (key == "TargetFPS") engineData.targetFPS = std::stoi(value);
        else if (key == "ResourceDirectory") engineData.resourceDirectory = value;
        else if (key == "EnableDevTools") engineData.enableDevTools = (value == "true" || value == "1");
        else if (key == "LockAspectRatio") engineData.lockAspectRatio = (value == "true" || value == "1");
    }

    void ConfigManager::ParseUserLine(const std::string& key, const std::string& value)
    {
        if (key == "WindowWidth") userData.windowWidth = std::stoi(value);
        else if (key == "WindowHeight") userData.windowHeight = std::stoi(value);
        else if (key == "Fullscreen") userData.fullscreen = (value == "true" || value == "1");
        else if (key == "VSync") userData.vSync = (value == "true" || value == "1");
        else if (key == "ShowFPS") userData.showFPS = (value == "true" || value == "1");

        else if (key == "MasterVolume") userData.masterVolume = std::stof(value);
        else if (key == "MusicVolume") userData.musicVolume = std::stof(value);
        else if (key == "SFXVolume") userData.sfxVolume = std::stof(value);

        else if (key == "Language") userData.language = value;
        else if (key == "ShadowQuality") userData.shadowQuality = std::stoi(value);
        else if (key == "AntiAliasing") userData.antiAliasing = std::stoi(value);
    }

    void ConfigManager::ApplyAndSaveUserSettings(const UserSettings& newSettings)
    {
        userData = newSettings;

        std::stringstream ss;
        ss << "[Screen]\n";
        ss << "WindowWidth=" << userData.windowWidth << "\n";
        ss << "WindowHeight=" << userData.windowHeight << "\n";
        ss << "Fullscreen=" << (userData.fullscreen ? "true" : "false") << "\n";
        ss << "VSync=" << (userData.vSync ? "true" : "false") << "\n";
        ss << "ShowFPS=" << (userData.showFPS ? "true" : "false") << "\n\n";

        ss << "[Audio]\n";
        ss << "MasterVolume=" << userData.masterVolume << "\n";
        ss << "MusicVolume=" << userData.musicVolume << "\n";
        ss << "SFXVolume=" << userData.sfxVolume << "\n\n";

        ss << "[System]\n";
        ss << "Language=" << userData.language << "\n";
        ss << "ShadowQuality=" << userData.shadowQuality << "\n";
        ss << "AntiAliasing=" << userData.antiAliasing << "\n";

        if (FileSystem::WriteText("user_settings.ini", ss.str()))
        {
            ENGINE_INFO("user_settings.ini saved.");
        }
        else
        {
            ENGINE_ERROR("Couldn't save user_settings.ini.");
        }

        SettingsChangedEvent event(userData);
        Application::Get().GetEventBus().Publish(event);
    }

    void ConfigManager::ApplyAndSaveEngineConfig(const EngineConfig& newConfig)
    {
        engineData = newConfig;

        Logger::SetLevel(engineData.logLevel);

        std::stringstream ss;
        ss << "Backend=" << engineData.backend << "\n";
        ss << "LogLevel=" << LogLevelToString(engineData.logLevel) << "\n";
        ss << "ReplayMode=" << ReplayModeToString(engineData.replayMode) << "\n";
        ss << "FixedTimeStep=" << engineData.fixedTimeStep << "\n";
        ss << "TargetFPS=" << engineData.targetFPS << "\n";
        ss << "ResourceDirectory=" << engineData.resourceDirectory << "\n";
        ss << "EnableDevTools=" << (engineData.enableDevTools ? "true" : "false") << "\n";
        ss << "LockAspectRatio=" << (engineData.enableDevTools ? "true" : "false") << "\n";

        if (FileSystem::WriteText("engine.ini", ss.str()))
        {
            ENGINE_INFO("engine.ini updated.");
        }
        else
        {
            ENGINE_ERROR("Couldn't save engine.ini.");
        }
    }
}