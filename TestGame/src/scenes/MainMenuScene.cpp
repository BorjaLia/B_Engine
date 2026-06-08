#include "MainMenuScene.h"

#include "core/Application.h"
#include "scenes/SceneManager.h"
#include "components/UIAnchorComponent.h"
#include "components/TextComponent.h"
#include "components/SpriteComponent.h"
#include "components/ButtonComponent.h"
#include "components/CameraComponent.h"
#include "components/ScriptComponent.h"
#include "utils/StringHash.h"
#include "debug/Debug.h"

#include "GameplayScene.h" 
#include "TestScene3D.h"
#include "../scripts/MouseScript.h"

void MainMenuScene::Build(Engine::SceneBuilder& builder)
{
    auto& rm = *Engine::Application::Get().GetResourceManager();

    Engine::Texture2D texButton = rm.GetTexture("res/sprites/NoTexture.png");
    Engine::Font* font = rm.GetFont("res/fonts/ReemKufiFunRegular.ttf", 48);

    // 1. Camera (Needed for rendering and Mouse Script logic)
    Engine::Node* camNode = builder.CreateNode("MenuCamera");
    auto* camComp = camNode->AddComponent<Engine::CameraComponent>(0.5f);

    // 2. Game Title
    {
        Engine::Node* node = builder.CreateNode("TitleText");
        node->AddComponent<Engine::UIAnchorComponent>(Engine::AnchorPreset::TopCenter, Engine::Vector2f(0.0f, 150.0f));
        auto* txt = node->AddComponent<Engine::TextComponent>(
            font, "Engine", 72.0f,
            Engine::Color{ 255, 255, 255, 255 },
            Engine::RenderLayer::UI);
        txt->alignment = Engine::TextAlignment::Center;
        txt->pivot = Engine::Pivot::TopCenter;
    }

    // 3. Play Button (2D)
    {
        Engine::Node* node = builder.CreateNode("PlayButton2D");

        node->AddComponent<Engine::UIAnchorComponent>(Engine::AnchorPreset::Center, Engine::Vector2f(0.0f, -60.0f)); // Movido arriba
        node->AddComponent<Engine::SpriteComponent>(texButton, Engine::Pivot::Center, Engine::Color{ 80, 80, 180, 200 });

        auto* txt = node->AddComponent<Engine::TextComponent>(
            font, " PLAY 2D", 36.0f,
            Engine::Color{ 255, 255, 255, 255 },
            Engine::RenderLayer::UI);
        txt->pivot = Engine::Pivot::LeftCenter;

        auto* btn = node->AddComponent<Engine::ButtonComponent>();
        btn->SetOnClick([]()
            {
                Engine::Application::Get().LoadScene<PlatformerScene>();
            });
    }

    // 4. Play Button (3D)
    {
        Engine::Node* node = builder.CreateNode("PlayButton3D");

        node->AddComponent<Engine::UIAnchorComponent>(Engine::AnchorPreset::Center, Engine::Vector2f(0.0f, 60.0f)); // Movido abajo
        node->AddComponent<Engine::SpriteComponent>(texButton, Engine::Pivot::Center, Engine::Color{ 180, 80, 80, 200 });

        auto* txt = node->AddComponent<Engine::TextComponent>(
            font, " PLAY 3D", 36.0f,
            Engine::Color{ 255, 255, 255, 255 },
            Engine::RenderLayer::UI);
        txt->pivot = Engine::Pivot::LeftCenter;

        auto* btn = node->AddComponent<Engine::ButtonComponent>();
        btn->SetOnClick([]()
            {
                Engine::Application::Get().LoadScene<TestScene3D>();
            });
    }

    // 5. Mouse Cursor
    {
        Engine::Node* node = builder.CreateNode("Mouse");
        node->AddComponent<Engine::SpriteComponent>(texButton, Engine::Pivot::Center, Engine::Color{ 80, 80, 80, 200 });
        node->transform.SetScale(Engine::Vector2f(0.2f, 0.2f));

        node->AddComponent<Engine::ScriptComponent>(new MouseScript(camComp));
    }
}