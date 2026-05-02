#include "SceneManager.h"

namespace Engine
{
    void SceneManager::LoadScene(std::unique_ptr<IScene> newScene)
    {
        pendingScene = std::move(newScene);
        hasPendingScene = true;
    }

    std::unique_ptr<IScene> SceneManager::ConsumePendingScene()
    {
        hasPendingScene = false;
        return std::move(pendingScene);
    }
}