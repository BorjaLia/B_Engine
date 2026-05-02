#pragma once

#include "scenes/Scene.h" 

class PlatformerScene : public Engine::IScene
{
public:
    void Build(Engine::SceneBuilder& builder) override;
};