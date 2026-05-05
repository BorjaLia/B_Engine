#pragma once

#include "scripts/Script.h"
#include "utils/Types.h"

namespace Engine
{
    class SpriteComponent;
}

/// Updates the minimap texture
class MiniMapScript : public Engine::Script
{
public:
    MiniMapScript(Engine::SpriteComponent* sprite);

    void OnStart() override;
    void OnUpdate(float deltaTime) override;

private:
    Engine::SpriteComponent* sprite;

    Engine::RenderTexture2D rendTex;
};