#include "Application.h"

#include "ConfigManager.h"
#include "../time/Time.h"
#include "../debug/Debug.h"
#include "../debug/DebugNode.h"
#include "../debug/MemoryTracker.h"
#include "../physics/PhysicsSystem.h"
#include "../components/AudioListenerComponent.h"
#include "../components/ColliderComponent.h"
#include "../components/TextComponent.h"
#include "../components/UiAnchorComponent.h"
#include "../components/CameraComponent.h"
#include "../inputs/KeyCodes.h"

#include "../platform/raylib/RaylibAudio.h"
#include "../platform/raylib/RaylibInput.h"
#include "../platform/raylib/RaylibRenderer.h"
#include "../platform/raylib/RaylibWindow.h"
#include "../platform/windowsConsole/ConsoleWindow.h"

#include <iostream>
#include <cmath>
#include <algorithm>

namespace Engine
{
    static BackendAPI StringToBackendAPI(std::string backendStr);

    Application* Application::instance = nullptr;

    Application::Application()
    {
        instance = this;
        rootScene = std::make_unique<Node>("Root");
        eventBus.Subscribe<WindowCloseEvent>([this](WindowCloseEvent& e) { OnWindowClose(e); });
    }

    Application::~Application() {}

    bool Application::Initialize(const std::string& title)
    {
        ConfigManager::LoadAll();

        const EngineConfig& engineConfig = ConfigManager::GetEngineConfig();

        Logger::Init();
        Logger::SetLevel(engineConfig.logLevel);

        currentEngineMode = engineConfig.replayMode;

        BackendAPI backend = StringToBackendAPI(engineConfig.backend);

        switch (backend)
        {
        case BackendAPI::Raylib:
            window = std::make_unique<RaylibWindow>();
            break;
        case BackendAPI::Console:
            window = std::make_unique<ConsoleWindow>();
            break;
        default:
            ENGINE_ERROR("Unknown backend API");
            return false;
        }

        ENGINE_INFO("Engine config loaded");

        const UserSettings& userSettings = ConfigManager::GetUserSettings();
        Vector2i windowSize(userSettings.windowWidth, userSettings.windowHeight);

        if (!window->Initialize(windowSize, title))
        {
            ENGINE_ERROR("Couldn't initialize window!");
            return false;
        }

        window->SetFullscreen(userSettings.fullscreen);
        window->SetVSync(userSettings.vSync);

        settingsEventId = eventBus.Subscribe<SettingsChangedEvent>(
            [this](SettingsChangedEvent& e) { this->OnSettingsChanged(e); }
        );

        window->SetEventCallback([this](Event& e) { eventBus.Publish(e); });

        auto raylibAudio = std::make_unique<RaylibAudio>();
        if (raylibAudio->Initialize())
        {
            audio = std::move(raylibAudio);
        }
        else
        {
            ENGINE_WARN("Application: Audio device failed to initialize. Audio disabled.");
        }

        muteEventId = eventBus.Subscribe<AudioMuteEvent>([this](AudioMuteEvent& e)
            {
                if (audio) audio->SetMasterMuted(e.GetMute());
            });

        volumeEventId = eventBus.Subscribe<AudioVolumeEvent>([this](AudioVolumeEvent& e)
            {
                if (audio) audio->SetMasterVolume(e.GetVolume());
            });

        if (audio) audio->SetMasterVolume(userSettings.masterVolume);

        resourceManager = std::make_unique<ResourceManager>(window->GetRenderer(), audio.get());
        physicsSystem = std::make_unique<PhysicsSystem>();
        debugNode = std::move(CreateDebugNode());

        inputManager.Initialize(window->GetInput());

        isRunning = true;
        return true;
    }

    void Application::OnSettingsChanged(SettingsChangedEvent& e)
    {
        const UserSettings& settings = e.GetSettings();

        ENGINE_INFO("Applying new user settings in real-time...");

        if (window)
        {
            window->SetSize({ settings.windowWidth, settings.windowHeight });
            window->SetFullscreen(settings.fullscreen);
            window->SetVSync(settings.vSync);
        }

        if (audio)
        {
            audio->SetMasterVolume(settings.masterVolume);
        }
    }

    bool Application::OnWindowClose(WindowCloseEvent& e)
    {
        isRunning = false;
        e.handled = true;
        return true;
    }

    void Application::UpdateEngineCaches()
    {
        activeCameras.clear();
        activeAudioListener = nullptr;

        if (rootScene)
        {
            rootScene->GetAllCameras(activeCameras);

            Node* listenerNode = FindListenerInTree(rootScene.get());
            if (listenerNode)
            {
                activeAudioListener = listenerNode->GetComponent<AudioListenerComponent>();
            }
        }

        ENGINE_LOG("Engine caches updated. Cameras found: {}", activeCameras.size());
    }

    void Application::ProcessPendingScene()
    {
        rootScene->ClearChildren();
        auto newScene = sceneManager.ConsumePendingScene();

        if (newScene)
        {
            newScene->Build(sceneBuilder);
        }
        timerManager.ClearAll();
    }

    void Application::RenderFrame(RendererBase* renderer, const Color& bgColor)
    {
        renderer->BeginFrame();
        renderer->ClearScreen(bgColor);

        for (CameraComponent* cam : activeCameras)
        {
            if (cam->HasRenderTarget())
            {
                renderer->BeginRenderToTexture(cam->GetRenderTarget());
                renderer->ClearScreen(bgColor);
            }

            Vector2f camPos = cam->GetOwner()->GetGlobalPosition();
            camPos.x = std::round(camPos.x);
            camPos.y = std::round(camPos.y);

            renderer->BeginCamera(camPos, cam->GetZoom());
            renderer->Flush(RenderLayer::World);

            if (cam->GetShowDebug()) renderer->FlushDebug(RenderLayer::World);

            renderer->EndCamera();

            if (cam->HasRenderTarget()) renderer->EndRenderToTexture();
        }

        renderer->Flush(RenderLayer::UI);
        if (debugMode) renderer->FlushDebug(RenderLayer::UI);

        renderer->EndFrame();
    }

    void Application::Run()
    {
        sceneBuilder.FlushToScene(rootScene.get());

        Color bgColor(50, 50, 60, 255);
        RendererBase* renderer = window->GetRenderer();
        InputBase* input = window->GetInput();

        const float FIXED_TIMESTEP = 1.0f / 60.0f;
        float accumulator = 0.0f;
        float fpsTimer = 0.0f;
        int frameCount = 0;
        int currentFPS = 0;

        if (currentEngineMode == EngineMode::AutoRecord)
        {
            ENGINE_INFO("AUTO-RECORD: Recording from the start");
            GetInputManager().GetLogger().StartRecording(true);
        }
        else if (currentEngineMode == EngineMode::AutoPlayback)
        {
            if (GetInputManager().GetInjector().LoadFromFile("replay_01.rep"))
            {
                ENGINE_INFO("AUTO-PLAYBACK: Replaying from the start");
                GetInputManager().GetInjector().Play();
            }
        }

        while (isRunning)
        {
            window->OnUpdate();
            if (!isRunning) break;

            inputManager.Update(eventBus);
            input->Update(eventBus);

            if (sceneManager.HasPendingScene())
            {
                ProcessPendingScene();
                accumulator = 0.0f;
            }

            sceneBuilder.FlushToScene(rootScene.get());

            float rawDt = renderer->GetDeltaTime();
            if (rawDt > 0.25f) rawDt = 0.25f;
            Time::Update(rawDt);
            timerManager.Update(Time::GetDeltaTime());

            fpsTimer += Time::GetUnscaledDeltaTime();
            if (++frameCount, fpsTimer >= 1.0f)
            {
                currentFPS = frameCount;
                frameCount = 0;
                fpsTimer -= 1.0f;
                if (debugMode) UpdateDebugNode(debugNode.get(), currentFPS);
            }

            accumulator += Time::GetUnscaledDeltaTime();

            if (isScenePendingStart)
            {
                rootScene->Start();
                UpdateEngineCaches();
                isScenePendingStart = false;
            }

            while (accumulator >= FIXED_TIMESTEP)
            {
                inputManager.FixedUpdate();
                float scaledTimestep = FIXED_TIMESTEP * Time::GetTimeScale();

                rootScene->FixedUpdate(scaledTimestep);
                if (physicsSystem) physicsSystem->Update(rootScene.get(), scaledTimestep);

                inputManager.PostFixedUpdate();
                accumulator -= FIXED_TIMESTEP;
                Time::AdvanceTick();
            }

            rootScene->Update(Time::GetDeltaTime());

            if (input->IsKeyReleased(Key::F3)) ToggleDebugMode();
            if (input->IsKeyReleased(Key::F4)) rootScene->DumpTree();

            if (input->IsKeyReleased(Key::F5))
            {
                if (!GetInputManager().GetLogger().IsRecording())
                {
                    GetInputManager().GetLogger().StartRecording(true);
                }
                else
                {
                    ENGINE_INPUT("Already recording! Use F6 to stop & save");
                }
            }

            if (input->IsKeyReleased(Key::F6))
            {
                GetInputManager().GetLogger().StopRecording();
                GetInputManager().GetLogger().SaveToFile("replay_01.rep");

                const auto& data = GetInputManager().GetLogger().GetRecordedData();
                ENGINE_INFO("--- SAVED REPLAY SUMMARY (F6) ---");
                for (const auto& entry : data)
                {
                    ENGINE_INFO("Tick: {} | Hash: {} | Val: {}", entry.tick, entry.actionHash, entry.value);
                }
            }

            if (input->IsKeyReleased(Key::F7))
            {
                if (GetInputManager().GetInjector().LoadFromFile("replay_01.rep"))
                {
                    const auto& data = GetInputManager().GetInjector().GetPlaybackData();
                    ENGINE_INFO("--- LOADED REPLAY SUMMARY (F7) ---");
                    for (const auto& entry : data)
                    {
                        ENGINE_INFO("Tick: {} | Hash: {} | Val: {}", entry.tick, entry.actionHash, entry.value);
                    }
                    ENGINE_INFO("--- REPLAY PLAYBACK: STARTED ---");
                    GetInputManager().GetInjector().Play();
                }
                else
                {
                    ENGINE_ERROR("Failed to start replay: File not found.");
                }
            }

            if (debugMode)
            {
                debugNode->Update(Time::GetDeltaTime());
                rootScene->DebugDraw(renderer);
            }

            rootScene->Draw(renderer);
            if (debugMode) debugNode->Draw(renderer);

            RenderFrame(renderer, bgColor);

            if (isSceneDirty)
            {
                rootScene->CleanUp();
                UpdateEngineCaches();
                isSceneDirty = false;
            }
        }
    }

    void Application::Shutdown()
    {
        eventBus.Unsubscribe(SettingsChangedEvent::GetStaticType(), settingsEventId);
        eventBus.Unsubscribe(AudioMuteEvent::GetStaticType(), muteEventId);
        eventBus.Unsubscribe(AudioVolumeEvent::GetStaticType(), volumeEventId);

        if (audio)  audio->Shutdown();
        if (window) window->Shutdown();
        isRunning = false;
    }

    void Application::ToggleDebugMode()
    {
        debugMode = !debugMode;
        debugNode->SetActive(debugMode);

        if (window && window->GetRenderer())
        {
            window->GetRenderer()->SetDebugRenderEnabled(debugMode);
        }
        ENGINE_LOG("Debug mode: {}", (debugMode ? "ON" : "OFF"));
    }

    Node* Application::FindAudioListener() const
    {
        return activeAudioListener ? activeAudioListener->GetOwner() : nullptr;
    }

    Node* Application::FindListenerInTree(Node* node)
    {
        if (!node) return nullptr;
        if (node->GetComponent<AudioListenerComponent>()) return node;
        for (const auto& child : node->GetChildren())
        {
            Node* found = FindListenerInTree(child.get());
            if (found) return found;
        }
        return nullptr;
    }

    static BackendAPI StringToBackendAPI(std::string backendStr)
    {
        std::transform(backendStr.begin(), backendStr.end(), backendStr.begin(),
            [](unsigned char c) { return static_cast<char>(std::tolower(c)); }
        );
        if (backendStr == "raylib")  return BackendAPI::Raylib;
        if (backendStr == "console") return BackendAPI::Console;

        ENGINE_WARN("Backend '{}' couldn't be recognized. Using Raylib...", backendStr);
        return BackendAPI::Raylib;
    }
}