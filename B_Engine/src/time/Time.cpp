#include "Time.h"

namespace Engine
{
    float Time::deltaTime = 0.0f;
    float Time::unscaledDeltaTime = 0.0f;
    float Time::timeScale = 1.0f;
    float Time::totalTime = 0.0f;

    int Time::totalFrames = 0;
    int Time::totalTicks = 0;

    void Time::Update(float rawDeltaTime)
    {
        unscaledDeltaTime = rawDeltaTime;
        deltaTime = rawDeltaTime;
        totalTime += rawDeltaTime;

        totalFrames++;
    }

    void Time::AdvanceTick()
    {
        totalTicks++;
    }
}