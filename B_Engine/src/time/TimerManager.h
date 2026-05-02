#pragma once

#include <vector>
#include <functional>
#include <cstdint>

namespace Engine
{
    using TimerID = uint32_t;

    /// Manages delayed execution callbacks and recurring timers.
    class TimerManager
    {
    public:
        TimerManager(const TimerManager&) = delete;
        TimerManager& operator=(const TimerManager&) = delete;
        TimerManager(TimerManager&&) = delete;
        TimerManager& operator=(TimerManager&&) = delete;

        /// Executes a callback function after a specified delay.
        /// @param seconds The time in seconds to wait before execution.
        /// @param callback The function to execute.
        /// @return A unique TimerID used to track or cancel the timer.
        TimerID SetTimeout(float seconds, std::function<void()> callback);

        /// Cancels an active or pending timer.
        /// @param id The unique identifier of the timer to remove.
        void ClearTimer(TimerID id);

        /// Clears all timers immediately (ideal for scene transitions).
        void ClearAll();

    private:
        friend class Application;

        TimerManager() = default;
        ~TimerManager() = default;

        void Update(float dt);

        struct TimerData
        {
            TimerID id;
            float timeRemaining;
            std::function<void()> callback;
        };

        std::vector<TimerData> activeTimers;
        std::vector<TimerData> pendingTimers;

        bool isUpdating = false;
        TimerID nextTimerId = 1;
    };
}