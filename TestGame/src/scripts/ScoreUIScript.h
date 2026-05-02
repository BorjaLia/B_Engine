#pragma once

#include <cstdint>

#include "scripts/Script.h"

namespace Engine { class EventBus; }

/// Listens for coin collection events and updates the attached text component.
class ScoreUIListener : public Engine::Script
{
public:
    ScoreUIListener() = default;
    ~ScoreUIListener() override = default;

    void OnStart() override;
    void OnDestroy() override;

private:
    Engine::EventBus* eventBus = nullptr;
    uint32_t listenerId = 0;
    int currentCoins = 0;
};