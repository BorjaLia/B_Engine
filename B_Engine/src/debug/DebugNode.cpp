#include "DebugNode.h"

#include <string>

#include "../core/Application.h"
#include "../components/TextComponent.h"
#include "../components/SpriteComponent.h"
#include "../components/UiAnchorComponent.h"
#include "../time/Time.h"
#include "../resources/ResourceManager.h"

namespace Engine
{
    std::unique_ptr<Node> CreateDebugNode()
    {
        ResourceManager* rm = Application::Get().GetResourceManager();

        std::unique_ptr<Node> rootDebug = std::make_unique<Node>("DebugRoot");
        rootDebug->AddComponent<UIAnchorComponent>();

        Font* debugFont = rm->GetFont("res/fonts/ReemKufiFunRegular.ttf", 48);

        std::unique_ptr<Node> backgroundNode = std::make_unique<Node>("Background");
        auto* sprite = backgroundNode->AddComponent<SpriteComponent>(rm->GetWhitePixel(), Pivot::TopLeft, Color(64, 64, 64, 200), RenderLayer::UI);
        sprite->SetTargetSize(Vector2(150.0f, 75.0f));
        rootDebug->AddChild(std::move(backgroundNode));

        std::unique_ptr<Node> fpsNode = std::make_unique<Node>("FPSCounter");
        fpsNode->AddComponent<TextComponent>(debugFont, "FPS: 0", 32.0f, Color(0, 255, 0, 255), RenderLayer::UI);
        fpsNode->transform->SetPosition({ 5.0f, 0.0f });
        rootDebug->AddChild(std::move(fpsNode));

        std::unique_ptr<Node> framesNode = std::make_unique<Node>("FrameCounter");
        framesNode->AddComponent<TextComponent>(debugFont, "Frames: 0", 32.0f, Color(0, 255, 0, 255), RenderLayer::UI);
        framesNode->transform->SetPosition({ 5.0f, 20.0f });
        rootDebug->AddChild(std::move(framesNode));

        std::unique_ptr<Node> ticksNode = std::make_unique<Node>("TickCounter");
        ticksNode->AddComponent<TextComponent>(debugFont, "Ticks: 0", 32.0f, Color(0, 255, 0, 255), RenderLayer::UI);
        ticksNode->transform->SetPosition({ 5.0f, 40.0f });
        rootDebug->AddChild(std::move(ticksNode));

        rootDebug->SetActive(false);

        return rootDebug;
    }

    void UpdateDebugNode(Node* debugNode, int currentFPS)
    {
        if (!debugNode || !debugNode->IsActive()) return;

        if (Node* n = debugNode->FindChild("FPSCounter"))
        {
            n->GetComponent<TextComponent>()->SetText("FPS: " + std::to_string(currentFPS));
        }

        if (Node* n = debugNode->FindChild("FrameCounter"))
        {
            n->GetComponent<TextComponent>()->SetText("Frames: " + std::to_string(Time::GetFrames()));
        }

        if (Node* n = debugNode->FindChild("TickCounter"))
        {
            n->GetComponent<TextComponent>()->SetText("Ticks: " + std::to_string(Time::GetTicks()));
        }
    }
}