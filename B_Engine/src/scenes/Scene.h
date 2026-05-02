#pragma once

#include "SceneBuilder.h"

namespace Engine
{
    /// Interface representing a distinct game state or level (e.g. MainMenu, Gameplay).
    /// @ingroup Scenes
    class IScene
    {
    public:
        virtual ~IScene() = default;

        /// Constructs the initial node hierarchy for this scene.
        /// @param builder The SceneBuilder used to instantiate nodes safely.
        virtual void Build(SceneBuilder& builder) = 0;

        /// Called immediately after the scene is loaded and becomes active.
        virtual void OnEnter() {}

        /// Called right before the scene is destroyed or replaced by another.
        virtual void OnExit() {}

        /// Called when the game state is paused.
        virtual void OnPause() {}

        /// Called when the game state is resumed from a pause.
        virtual void OnResume() {}
    };
}