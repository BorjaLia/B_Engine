#pragma once

#include "scripts/Script.h"
#include "utils/Types.h"

#include "components/CameraComponent.h"
#include "components/SpriteComponent.h"
#include "components/ButtonComponent.h"

/// Updates the minimap texture
class MiniMapScript : public Engine::Script
{
public:
    MiniMapScript(Engine::CameraComponent* cam,
        Engine::SpriteComponent* mapSprite,
        Engine::SpriteComponent* borderSprite,
        Engine::ButtonComponent* resizeBtn,
        float minSize = 100.0f,
        float maxSize = 500.0f);

    void OnStart() override;
    void OnUpdate(float deltaTime) override;

private:
    Engine::CameraComponent* cam;
    Engine::SpriteComponent* mapSprite;
    Engine::SpriteComponent* borderSprite;
    Engine::ButtonComponent* resizeBtn;

    Engine::RenderTexture2D rendTex;

    bool isDragging = false;
    Engine::Vector2f lastMousePos;

    float minSize;
    float maxSize;
    float currentSize;
};