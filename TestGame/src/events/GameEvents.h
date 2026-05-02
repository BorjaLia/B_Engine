#pragma once

#include "events/Event.h"

/// Triggered when the player collects a coin in the world.
/// @ingroup GameScripts
class CoinCollectedEvent : public Engine::EventBase<CoinCollectedEvent>
{
public:
    CoinCollectedEvent(int value = 1) : coinValue(value) {}

    int GetValue() const { return coinValue; }

    const char* GetName() const override { return "CoinCollectedEvent"; }

private:
    int coinValue;
};