#pragma once

#include "core/interfaces/IGame.h"

class PlatformerGame : public Engine::IGame
{
public:
    PlatformerGame() = default;
    ~PlatformerGame() override = default;

    void SetupInputs() override;
    void RegisterScenes() override;
};