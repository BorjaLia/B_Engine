#pragma once

#include <string>

#include "scenes/SceneBuilder.h"
#include "utils/Types.h"

/// Factory class to assemble static and dynamic environment entities.
class EnvironmentPrefabs
{
public:
    static Engine::Node* CreatePlatform(Engine::SceneBuilder& builder, const std::string& name, Engine::Texture2D tex, Engine::Vector2f pos, Engine::Vector2f scale);
    static void          CreateCoin(Engine::SceneBuilder& builder, Engine::Texture2D tex, Engine::Vector2f pos);
    static Engine::Node* CreateBox(Engine::SceneBuilder& builder, Engine::Texture2D tex, Engine::Vector2f pos);
    static Engine::Node* CreateEnemy(Engine::SceneBuilder& builder, Engine::Texture2D tex, Engine::Vector2f pos, float leftBound, float rightBound);
    static Engine::Node* CreateFinishLine(Engine::SceneBuilder& builder, Engine::Texture2D tex, Engine::Vector2f pos);
};