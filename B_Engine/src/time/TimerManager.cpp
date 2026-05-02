#include "TimerManager.h"

#include <algorithm>

namespace Engine
{
    TimerID TimerManager::SetTimeout(float seconds, std::function<void()> callback)
    {
        TimerID id = nextTimerId++;
        if (isUpdating)
        {
            pendingTimers.push_back({ id, seconds, callback });
        }
        else
        {
            activeTimers.push_back({ id, seconds, callback });
        }
        return id;
    }

    void TimerManager::ClearTimer(TimerID id)
    {
        if (id == 0) return;

        // Remove from active list
        activeTimers.erase(
            std::remove_if(activeTimers.begin(), activeTimers.end(),
                [id](const TimerData& t) { return t.id == id; }),
            activeTimers.end()
        );

        // Remove from pending list
        pendingTimers.erase(
            std::remove_if(pendingTimers.begin(), pendingTimers.end(),
                [id](const TimerData& t) { return t.id == id; }),
            pendingTimers.end()
        );
    }

    void TimerManager::Update(float dt)
    {
        isUpdating = true;

        for (auto it = activeTimers.begin(); it != activeTimers.end(); )
        {
            it->timeRemaining -= dt;
            if (it->timeRemaining <= 0.0f)
            {
                if (it->callback) it->callback();
                it = activeTimers.erase(it); // Erase and move to the next
            }
            else
            {
                ++it;
            }
        }

        isUpdating = false;

        // Add any timers that were queued during the update loop
        if (!pendingTimers.empty())
        {
            activeTimers.insert(activeTimers.end(), pendingTimers.begin(), pendingTimers.end());
            pendingTimers.clear();
        }
    }

    void TimerManager::ClearAll()
    {
        activeTimers.clear();
        pendingTimers.clear();
    }
}