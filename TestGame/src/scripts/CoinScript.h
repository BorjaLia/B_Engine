#pragma once

#include "scripts/Script.h"

/// Simple behavior for a collectible entity.
class CoinScript : public Engine::Script
{
public:
    CoinScript() = default;
    ~CoinScript() override = default;

    void OnStart() override;
};