#include "GameplayScene.h"

#include "core/Application.h"
#include "time/Time.h"
#include "scenes/SceneManager.h"
#include "components/ScriptComponent.h"

#include "../scripts/GameManager.h"
#include "../prefabs/PlayerPrefab.h"
#include "../prefabs/EnvironmentPrefabs.h"
#include "../prefabs/UIPrefabs.h"

void PlatformerScene::Build(Engine::SceneBuilder& builder)
{
    auto& app = Engine::Application::Get();
    auto& rm = *app.GetResourceManager();

    Engine::Time::SetTimeScale(1.0f);

    // Load Resources
    Engine::Texture2D texPlatform = rm.GetTexture("res/sprites/platform.png");
    Engine::Texture2D texBox = rm.GetTexture("res/sprites/box.png");
    Engine::Texture2D texEnemy = rm.GetTexture("res/sprites/enemy.png");
    Engine::Texture2D texButton = rm.GetTexture("res/sprites/NoTexture.png");
    Engine::Texture2D animPlayer = rm.GetTexture("res/sprites/CharacterSheet.png");
    Engine::Texture2D animCoin = rm.GetTexture("res/sprites/CoinAnimation.png");
    Engine::Font* font = rm.GetFont("res/fonts/ReemKufiFunRegular.ttf", 48);

    // 1. Instantiate Systems
    // FIX: Using ScriptComponent to properly attach a user Script
    builder.CreateNode("GameManager")->AddComponent<Engine::ScriptComponent>(new GameManager());

    // 2. Build World
    EnvironmentPrefabs::CreatePlatform(builder, "Ground", texPlatform, { -200.0f, 100.0f }, { 32.0f, 1.0f });
    EnvironmentPrefabs::CreatePlatform(builder, "PlatformA", texPlatform, { 550.0f, 300.0f }, { 4.0f, 0.5f });
    EnvironmentPrefabs::CreatePlatform(builder, "PlatformB", texPlatform, { 1200.0f, 400.0f }, { 4.0f, 0.5f });
    EnvironmentPrefabs::CreatePlatform(builder, "PlatformC", texPlatform, { 1000.0f, 550.0f }, { 4.0f, 0.5f });

    // 3. Dynamic Actors
    PlayerPrefab::Create(builder, { 150.0f, 350.0f });
    EnvironmentPrefabs::CreateBox(builder, texBox, { 500.0f, 450.0f });
    EnvironmentPrefabs::CreateEnemy(builder, texEnemy, { 1050.0f, 560.0f }, 870.0f, 1230.0f);

    // 4. Objectives
    EnvironmentPrefabs::CreateCoin(builder, animCoin, { 400.0f, 350.0f });
    EnvironmentPrefabs::CreateCoin(builder, animCoin, { 800.0f, 650.0f });
    EnvironmentPrefabs::CreateCoin(builder, animCoin, { 1200.0f, 500.0f });
    EnvironmentPrefabs::CreateFinishLine(builder, texPlatform, { 1550.0f, 300.0f });

    // 5. UI and Mouse
    UIPrefabs::CreateLevelHUD(builder, font, texButton);
    UIPrefabs::CreateMouse(builder, font, texButton);
}