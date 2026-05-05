#pragma once

#include "scripts/Script.h"
#include "utils/Types.h"

#include "components/CameraComponent.h"
#include "components/SpriteComponent.h"

/// Updates the minimap texture
class MiniMapScript : public Engine::Script
{
public:
    MiniMapScript(Engine::CameraComponent* cam, Engine::SpriteComponent* sprite);

    void OnStart() override;
    void OnUpdate(float deltaTime) override;

private:
    Engine::CameraComponent* cam;
    Engine::SpriteComponent* sprite;

    Engine::RenderTexture2D rendTex;
};