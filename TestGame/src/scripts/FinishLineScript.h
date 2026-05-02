#pragma once

#include "scripts/Script.h"

/// Triggers the win condition when the player reaches the end of the level.
class FinishLineScript : public Engine::Script
{
public:
    FinishLineScript() = default;
    ~FinishLineScript() override = default;

    void OnStart() override;
};