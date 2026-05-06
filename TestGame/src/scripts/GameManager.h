#pragma once

#include <string>

#include "scripts/Script.h"
#include "random/Random.h"

namespace Engine
{
    class TextComponent;
    class RigidBodyComponent;
}

/// Global manager for the current game session.
/// Handles score, win/lose states, and coordinates with the replay system for deterministic RNG.
class GameManager : public Engine::Script
{
public:
    static GameManager* Get() { return instance; }

    void OnStart() override;
    void OnFixedUpdate(float deltaTime) override;

    void CollectCoin();
    void Win();
    void Lose();
    void TogglePause();

    bool IsPaused() const { return paused; }
    bool IsOver()   const { return gameWon || gameLost; }
    int  GetCoins() const { return coins; }

    Engine::Random& GetRandom() { return gameplayRandom; }

private:
    inline static GameManager* instance = nullptr;

    int  coins = 0;
    bool paused = false;
    bool gameWon = false;
    bool gameLost = false;

    Engine::TextComponent* scoreText = nullptr;
    Engine::TextComponent* stateText = nullptr;
    Engine::RigidBodyComponent* playerBody = nullptr;

    Engine::Random gameplayRandom;

    void ShowStateText(const std::string& msg);
    void HideStateText();
};