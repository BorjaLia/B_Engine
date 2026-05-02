#pragma once

#include <memory>
#include "Scene.h"

namespace Engine
{
    /// Handles the lifecycle and transition of game scenes.
    /// @ingroup Scenes
    class SceneManager
    {
    public:
        SceneManager(const SceneManager&) = delete;
        SceneManager& operator=(const SceneManager&) = delete;
        SceneManager(SceneManager&&) = delete;
        SceneManager& operator=(SceneManager&&) = delete;

        /// Queues a new scene to be loaded at the end of the current frame.
        void LoadScene(std::unique_ptr<IScene> newScene);

        /// Checks if a scene transition is pending.
        bool HasPendingScene() const { return hasPendingScene; }

        /// Extracts the queued scene and lowers the transition flag.
        std::unique_ptr<IScene> ConsumePendingScene();

    private:
        friend class Application;

        SceneManager() = default;
        ~SceneManager() = default;

        std::unique_ptr<IScene> pendingScene = nullptr;
        bool hasPendingScene = false;
    };
}