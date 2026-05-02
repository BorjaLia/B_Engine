#pragma once

#include "scenes/Scene.h"

class MainMenuScene : public Engine::IScene
{
public:
    void Build(Engine::SceneBuilder& builder) override;
};