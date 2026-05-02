#pragma once

#include "scenes/SceneBuilder.h"
#include "utils/Types.h"

/// Factory class for assembling the Player entity.
class PlayerPrefab
{
public:
    /// Assembles the player and its camera, returning a pointer to the main node.
    static Engine::Node* Create(Engine::SceneBuilder& builder, Engine::Vector2f position);
};