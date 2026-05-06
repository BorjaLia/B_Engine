#include "GameManager.h"

#include "scenes/Node.h"
#include "core/Application.h"
#include "components/TextComponent.h"
#include "components/RigidBodyComponent.h"
#include "time/Time.h"
#include "utils/StringHash.h"

void GameManager::OnStart()
{
    instance = this;

    if (auto* n = Engine::Application::Get().GetRootScene()->FindChild("ScoreText"))
    {
        scoreText = n->GetComponent<Engine::TextComponent>();
    }

    if (auto* n = Engine::Application::Get().GetRootScene()->FindChild("StateText"))
    {
        stateText = n->GetComponent<Engine::TextComponent>();
        n->SetActive(false);
    }

    if (auto* playerNode = Engine::Application::Get().GetRootScene()->FindChild("Player"))
    {
        playerBody = playerNode->GetComponent<Engine::RigidBodyComponent>();
    }

    // =========================================================
    // REPLAY SYSTEM CONNECTION (DETERMINISTIC RNG)
    // =========================================================
    auto& inputManager = Engine::Application::Get().GetInputManager();

    // When the Logger wants to save, we provide the current state of our RNG
    inputManager.GetLogger().SetMetadataProvider([this]()
        {
            ENGINE_INFO("GameManager: InputLogger requested RNG state. Sending...");
            return this->gameplayRandom.GetState();
        });

    // When the Injector loads a file, we overwrite our RNG state with the loaded one
    inputManager.GetInjector().SetMetadataConsumer([this](const std::string& data)
        {
            ENGINE_INFO("GameManager: InputInjector provided RNG state. Overwriting...");
            this->gameplayRandom.SetState(data);
        });
}

void GameManager::OnFixedUpdate(float)
{
    auto& mapper = Engine::Application::Get().GetInputManager();
   
    if (mapper.IsActionJustReleased(Engine::Hash::GetHash("Fullscreen")))  Engine::Application::Get().GetWindow()->SetFullscreen(!Engine::Application::Get().GetWindow()->IsFullscreen());
}

void GameManager::CollectCoin()
{
    if (IsOver()) return;

    coins++;

    if (scoreText)
    {
        scoreText->SetText("Coins: " + std::to_string(coins));
    }
}

void GameManager::Win()
{
    if (IsOver()) return;

    gameWon = true;
    ShowStateText("YOU WIN!");
}

void GameManager::Lose()
{
    if (IsOver()) return;

    gameLost = true;
    ShowStateText("GAME OVER");
    Engine::Time::SetTimeScale(0.0f);
}

void GameManager::TogglePause()
{
    if (IsOver()) return;

    paused = !paused;

    if (paused)
    {
        ShowStateText("PAUSED");
        Engine::Time::SetTimeScale(0.0f);
    }
    else
    {
        HideStateText();
        Engine::Time::SetTimeScale(1.0f);
    }
}

void GameManager::ShowStateText(const std::string& msg)
{
    if (!stateText) return;

    stateText->SetText(msg);
    stateText->GetOwner()->SetActive(true);
}

void GameManager::HideStateText()
{
    if (stateText)
    {
        stateText->GetOwner()->SetActive(false);
    }
}