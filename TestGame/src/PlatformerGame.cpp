#include "PlatformerGame.h"

#include "core/Application.h"
#include "utils/StringHash.h"

// Scenes
#include "scenes/GameplayScene.h"
#include "scenes/MainMenuScene.h"

void PlatformerGame::SetupInputs()
{
    auto& input = Engine::Application::Get().GetInputManager();

    // =========================================================
    // UI & SYSTEM
    // =========================================================
    input.BindAction(Engine::Hash::GetHash("UI_Interact"), Engine::MouseButton::Left, Engine::ActionCategory::System);
    input.BindAction(Engine::Hash::GetHash("UI_Pause"), Engine::Key::Escape, Engine::ActionCategory::Gameplay);
    input.BindAction(Engine::Hash::GetHash("UI_Pause"), Engine::Key::Tab, Engine::ActionCategory::System);

    // =========================================================
    // GAMEPLAY
    // =========================================================
    input.BindAction(Engine::Hash::GetHash("Game_SpawnRight"), Engine::MouseButton::Right, Engine::ActionCategory::Gameplay);

    // Horizontal Movement
    input.BindAxis(Engine::Hash::GetHash("Game_MoveX"), Engine::Key::D, 1.0f);
    input.BindAxis(Engine::Hash::GetHash("Game_MoveX"), Engine::Key::A, -1.0f);
    input.BindAxis(Engine::Hash::GetHash("Game_MoveX"), Engine::Key::Right, 1.0f);
    input.BindAxis(Engine::Hash::GetHash("Game_MoveX"), Engine::Key::Left, -1.0f);

    // Jump
    input.BindAction(Engine::Hash::GetHash("Game_Jump"), Engine::Key::Space);
    input.BindAction(Engine::Hash::GetHash("Game_Jump"), Engine::Key::W);
    input.BindAction(Engine::Hash::GetHash("Game_Jump"), Engine::Key::Up);
    input.BindAction(Engine::Hash::GetHash("Game_Jump"), Engine::MouseButton::Middle);

    // Run
    input.BindAction(Engine::Hash::GetHash("Game_Run"), Engine::Key::LeftShift);

    // Rotation
    input.BindAxis(Engine::Hash::GetHash("Game_RotX"), Engine::Key::E, 1.0f);
    input.BindAxis(Engine::Hash::GetHash("Game_RotX"), Engine::Key::Q, -1.0f);
}

void PlatformerGame::RegisterScenes()
{
    // Start with the Main Menu
    Engine::Application::Get().GetSceneManager().LoadScene(std::make_unique<MainMenuScene>());
}