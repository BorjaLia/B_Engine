#pragma once

#include "scenes/SceneBuilder.h"

namespace Engine
{
    class Font;
    struct Texture2D;
}

/// Factory class to assemble User Interface entities.
class UIPrefabs
{
public:
    static void CreateLevelHUD(Engine::SceneBuilder& builder, Engine::Font* font, Engine::Texture2D btnTex);
    static void CreateMouse(Engine::SceneBuilder& builder, Engine::Font* font, Engine::Texture2D btnTex);
};