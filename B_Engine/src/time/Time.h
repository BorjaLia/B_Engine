#pragma once

namespace Engine
{
    /// Static utility class to manage engine time, delta time, and time scaling.
    class Time
    {
    public:
        // Prevent instantiation
        Time() = delete;

        /// Returns the scaled delta time. Used by game logic (affected by time scale).
        static float GetDeltaTime() { return deltaTime * timeScale; }

        /// Returns the unscaled, real-world delta time. Useful for UI or fixed logic.
        static float GetUnscaledDeltaTime() { return unscaledDeltaTime; }

        /// Returns the total scaled time elapsed since engine start.
        static float GetTime() { return totalTime; }

        static int GetFrames() { return totalFrames; }

        static int GetTicks() { return totalTicks; }
        static void ResetTicks() { totalTicks = 0; }

        static float GetTimeScale() { return timeScale; }
        static void SetTimeScale(float scale) { timeScale = scale; }

        /// Updates the time state. Expected to be called once per frame by the Application.
        /// @param rawDeltaTime The actual time passed since the last frame.
        static void Update(float rawDeltaTime);

        /// Advances the fixed physics tick counter.
        static void AdvanceTick();

    private:
        static float deltaTime;
        static float unscaledDeltaTime;
        static float timeScale;
        static float totalTime;
        static int totalFrames;
        static int totalTicks;
    };
}