///
/// B_Engine v 1.0
/// Borja Lia (https://www.github.com/BorjaLia)
///
/// 

#pragma once

#include <memory>
#include <string>
#include <vector>
#include <functional>

#include "interfaces/IWindow.h"
#include "interfaces/IAudio.h"
#include "../graphics/RendererBase.h"
#include "ReplayModes.h"
#include "../utils/Math.h"
#include "../utils/Types.h"
#include "../time/Time.h"
#include "../time/TimerManager.h"
#include "../events/EventBus.h"
#include "../events/ApplicationEvents.h"
#include "../events/AudioEvents.h"
#include "../inputs/InputManager.h"
#include "../resources/ResourceManager.h"
#include "../scenes/Scene.h"
#include "../scenes/SceneBuilder.h"
#include "../scenes/SceneManager.h"
#include "../scenes/Node.h"
#include "../debug/Debug.h"

namespace Engine
{
    enum class BackendAPI
    {
        Raylib,
        Console
    };

    class PhysicsSystem;

    /// @defgroup Core Core Systems
    /// @brief The heart of the engine (Application, Window, Time, Config, etc.).
    ///
    /// The core orchestrator of the engine.
    /// Manages systems initialization, the main game loop, and fixed-timestep physics.
    /// @ingroup Core
    class Application
    {
    public:
        Application();
        virtual ~Application();

        static Application& Get() { return *instance; }

#pragma region Lifecycle
        /// Initializes window, renderer, audio, and reads configuration files.
        /// @param title The name that will appear on the window bar.
        bool Initialize(const std::string& title = "Engine");

        /// Flushes the SceneBuilder, then locks into the main game loop.
        void Run();

        /// Releases all systems and subscriptions cleanly.
        void Shutdown();
#pragma endregion

#pragma region Managers & Accessors
        InputManager& GetInputManager() { return inputManager; }
        SceneBuilder& GetSceneBuilder() { return sceneBuilder; }
        SceneManager& GetSceneManager() { return sceneManager; }
        TimerManager& GetTimerManager() { return timerManager; }

        ResourceManager* GetResourceManager() const { return resourceManager.get(); }
        EventBus& GetEventBus() { return eventBus; }
        Node* GetRootScene() const { return rootScene.get(); }
        IWindow* GetWindow() const { return window.get(); }
        RendererBase* GetRenderer() const { return window->GetRenderer(); }
        IAudio* GetAudio() const { return audio.get(); }
#pragma endregion

#pragma region Engine State
        bool IsDebugMode() const { return debugMode; }
        void ToggleDebugMode();

        void MarkSceneDirty() { isSceneDirty = true; }
        void MarkScenePendingStart() { isScenePendingStart = true; }

        /// Requests a transition to a new scene type.
        /// @tparam T The class of the new scene to be loaded.
        template<typename T>
        void LoadScene()
        {
            sceneFactory = []() { return std::make_unique<T>(); };
            sceneManager.LoadScene(sceneFactory());
        }

        /// Reloads the exact same scene that is currently running.
        void ReloadCurrentScene()
        {
            if (sceneFactory)
            {
                sceneManager.LoadScene(sceneFactory());
            }
            else
            {
                ENGINE_WARN("No loaded scene to reload.");
            }
        }

        /// Walks the scene tree looking for an AudioListenerComponent.
        Node* FindAudioListener() const;
#pragma endregion

    private:
        static Application* instance;

        EngineMode currentEngineMode = EngineMode::Normal;

        std::unique_ptr<IWindow>         window;
        std::unique_ptr<IAudio>          audio;
        std::unique_ptr<ResourceManager> resourceManager;
        std::unique_ptr<PhysicsSystem>   physicsSystem;

        EventBus     eventBus;
        InputManager inputManager;
        SceneBuilder sceneBuilder;
        SceneManager sceneManager;
        TimerManager timerManager;

        std::function<std::unique_ptr<IScene>()> sceneFactory;

        std::unique_ptr<Node> rootScene;
        std::unique_ptr<Node> debugNode;

        bool isRunning = false;
        bool isSceneDirty = false;
        bool isScenePendingStart = true;
        bool debugMode = false;

        uint32_t settingsEventId = 0;
        uint32_t muteEventId = 0;
        uint32_t volumeEventId = 0;

        std::vector<class CameraComponent*> activeCameras;
        class AudioListenerComponent* activeAudioListener = nullptr;

        void UpdateEngineCaches();
        void ProcessPendingScene();
        void RenderFrame(class RendererBase* renderer, const Color& bgColor);

        // Loop Refactoring Methods
        void CalculateTime(float& accumulator, int& frameCount, float& fpsTimer, int& currentFPS, RendererBase* renderer);
        void UpdateFixed(float fixedTimestep, float& accumulator);
        void UpdateVariable();
        void HandleSystemHotkeys(InputBase* input);
        void RenderAndCleanup(RendererBase* renderer, const Color& bgColor);

        void OnSettingsChanged(SettingsChangedEvent& e);
        bool OnWindowClose(WindowCloseEvent& e);

        static Node* FindListenerInTree(Node* node);
    };
}