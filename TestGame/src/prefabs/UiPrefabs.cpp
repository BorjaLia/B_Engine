#include "UIPrefabs.h"

#include "scenes/Node.h"

// Components
#include "components/UIAnchorComponent.h"
#include "components/TextComponent.h"
#include "components/SpriteComponent.h"
#include "components/ButtonComponent.h"
#include "components/ScriptComponent.h"
#include "components/CameraComponent.h"

// Scripts & Logic
#include "../scripts/ScoreUIScript.h"
#include "../scripts/GameManager.h"
#include "../scripts/MouseScript.h"

void UIPrefabs::CreateLevelHUD(Engine::SceneBuilder& builder, Engine::Font* font, Engine::Texture2D btnTex)
{
    // 1. SCORE
    Engine::Node* nodeScore = builder.CreateNode("ScoreText");
    nodeScore->AddComponent<Engine::UIAnchorComponent>(Engine::AnchorPreset::TopRight, Engine::Vector2f(-20.0f, 20.0f));

    auto* txtScore = nodeScore->AddComponent<Engine::TextComponent>(font, "Coins: 0", 48.0f, Engine::Color{ 255, 230, 50, 255 }, Engine::RenderLayer::UI);
    txtScore->alignment = Engine::TextAlignment::Right;
    txtScore->pivot = Engine::Pivot::TopRight;

    nodeScore->AddComponent<Engine::ScriptComponent>(new ScoreUIListener());

    // 2. PAUSE BUTTON
    Engine::Node* nodePause = builder.CreateNode("PauseButton");
    nodePause->AddComponent<Engine::UIAnchorComponent>(Engine::AnchorPreset::TopLeft);
    nodePause->AddComponent<Engine::SpriteComponent>(btnTex, Engine::Pivot::TopLeft, Engine::Color{ 80, 80, 80, 200 });

    auto* txtPause = nodePause->AddComponent<Engine::TextComponent>(font, "  PAUSE", 36.0f, Engine::Color{ 255, 255, 255, 255 }, Engine::RenderLayer::UI);
    txtPause->pivot = Engine::Pivot::TopLeft;

    auto* btn = nodePause->AddComponent<Engine::ButtonComponent>();
    btn->SetOnClick([]()
        {
            if (GameManager::Get()) GameManager::Get()->TogglePause();
        });

    // 3. STATE OVERLAY (WIN/LOSE/PAUSE)
    Engine::Node* nodeState = builder.CreateNode("StateText");
    nodeState->AddComponent<Engine::UIAnchorComponent>(Engine::AnchorPreset::Center, Engine::Vector2f(0.0f, 0.0f));

    auto* txtState = nodeState->AddComponent<Engine::TextComponent>(font, "", 72.0f, Engine::Color{ 255, 255, 255, 255 }, Engine::RenderLayer::UI);
    txtState->alignment = Engine::TextAlignment::Center;
    txtState->pivot = Engine::Pivot::Center;
}

void UIPrefabs::CreateMouse(Engine::SceneBuilder& builder, Engine::Font* font, Engine::Texture2D btnTex)
{
    Engine::Node* node = builder.CreateNode("Mouse");

    auto* sprite = node->AddComponent<Engine::SpriteComponent>(btnTex, Engine::Pivot::Center, Engine::Color{ 80, 80, 80, 200 });
    sprite->SetTargetSize(Engine::Vector2f(50.0f, 50.0f));

    node->AddComponent<Engine::TextComponent>(font, "I", 72.0f, Engine::Color{ 255, 255, 255, 255 }, Engine::RenderLayer::World);

    // Find the camera to pass to the mouse script
    Engine::CameraComponent* camComp = nullptr;
    if (Engine::Node* camNode = builder.FindPending("Camera"))
    {
        camComp = camNode->GetComponent<Engine::CameraComponent>();
    }

    node->AddComponent<Engine::ScriptComponent>(new MouseScript(camComp));
}