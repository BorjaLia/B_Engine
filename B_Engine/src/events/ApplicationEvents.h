#pragma once

#include "Event.h"
#include "../math/Vector2.h"
#include "../core/ConfigManager.h"

namespace Engine
{
    /// Triggered when the OS window is resized. Contains the new dimensions.
    /// @ingroup Events
    class WindowResizeEvent : public EventBase<WindowResizeEvent>
    {
    public:
        WindowResizeEvent(const Vector2i& newSize) : size(newSize) {}
        WindowResizeEvent(int width, int height) : size(width, height) {}

        Vector2i GetSize() const { return size; }

        const char* GetName() const override { return "WindowResizeEvent"; }

    private:
        Vector2i size;
    };

    /// Triggered when the application window is requested to close by the OS.
    /// @ingroup Events
    class WindowCloseEvent : public EventBase<WindowCloseEvent>
    {
    public:
        WindowCloseEvent() = default;

        const char* GetName() const override { return "WindowCloseEvent"; }
    };

    /// Triggered when user settings (like volume, resolution) are applied and saved.
    /// @ingroup Events
    class SettingsChangedEvent : public EventBase<SettingsChangedEvent>
    {
    public:
        SettingsChangedEvent(const UserSettings& settings) : newSettings(settings) {}

        const UserSettings& GetSettings() const { return newSettings; }

        const char* GetName() const override { return "SettingsChangedEvent"; }

    private:
        UserSettings newSettings;
    };

    class ReplayStateEvent : public EventBase<ReplayStateEvent>
    {
    public:
        ReplayStateEvent(bool isPlaying) : playing(isPlaying) {}
        bool IsPlaying() const { return playing; }
        const char* GetName() const override { return "ReplayStateEvent"; }

    private:
        bool playing;
    };

}