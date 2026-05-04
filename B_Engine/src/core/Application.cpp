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

        window->GetRenderer()->SetLogicalResolution(windowSize);

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
        
            if (!GetInputManager().GetInjector().IsPlaying())
            {
                window->GetRenderer()->SetLogicalResolution({ settings.windowWidth, settings.windowHeight });
            }
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
        const bool useGlobalCanvas = renderer->IsUsingGlobalCanvas();

        RenderTexture2D globalCanvas;
        Vector2f lbOffset;
        float lbScale = 1.0f;

        if (useGlobalCanvas)
        {
            globalCanvas = renderer->GetGlobalCanvas();
            lbOffset = renderer->GetLetterboxOffset();
            lbScale = renderer->GetLetterboxScale();
        }

        renderer->BeginFrame();

        if (useGlobalCanvas)
        {
            renderer->ClearScreen({ 0, 0, 0, 255 });
            renderer->BeginRenderToTexture(globalCanvas);
        }
        renderer->ClearScreen(bgColor);

        for (CameraComponent* cam : activeCameras)
        {
            if (!cam->GetOwner()->IsActive()) continue;

            const bool isCameraRT = cam->HasRenderTarget();

            if (isCameraRT)
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

            if (isCameraRT)
            {
                renderer->EndRenderToTexture();

                if (useGlobalCanvas)
                {
                    renderer->BeginRenderToTexture(globalCanvas);
                }
            }
        }

        renderer->Flush(RenderLayer::UI);
        if (debugMode) renderer->FlushDebug(RenderLayer::UI);

        if (useGlobalCanvas)
        {
            renderer->EndRenderToTexture();
            renderer->DrawRenderTexture(
                renderer->GetGlobalCanvas(),
                renderer->GetLetterboxOffset(),
                { renderer->GetLetterboxScale(), renderer->GetLetterboxScale() },
                { 255, 255, 255, 255 }
            );
        }

        renderer->EndFrame();
    }

    void Application::CalculateTime(float& accumulator, int& frameCount, float& fpsTimer, int& currentFPS, RendererBase* renderer)
    {
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
            if (showDebugNode) UpdateDebugNode(debugNode.get(), currentFPS);
        }

        accumulator += Time::GetUnscaledDeltaTime();
    }

    void Application::UpdateFixed(float fixedTimestep, float& accumulator)
    {
        while (accumulator >= fixedTimestep)
        {
            inputManager.FixedUpdate();
            float scaledTimestep = fixedTimestep * Time::GetTimeScale();

            rootScene->FixedUpdate(scaledTimestep);
            if (physicsSystem) physicsSystem->Update(rootScene.get(), scaledTimestep);

            inputManager.PostFixedUpdate();
            accumulator -= fixedTimestep;
            Time::AdvanceTick();
        }
    }

    void Application::UpdateVariable()
    {
        rootScene->Update(Time::GetDeltaTime());
        if (showDebugNode) debugNode->Update(Time::GetDeltaTime());
    }

    void Application::HandleSystemHotkeys(InputBase* input)
    {
        if (input->IsKeyReleased(Key::F2)) rootScene->DumpTree();
        if (input->IsKeyReleased(Key::F3)) ToggleDebugNode();
        if (input->IsKeyReleased(Key::F4)) ToggleDebugMode();

        InputLogger& logger = GetInputManager().GetLogger();

        if (input->IsKeyReleased(Key::F5))
        {
            if (!logger.IsRecording())
            {
                logger.StartRecording(true);
            }
            else
            {
                ENGINE_INPUT("Already recording! Use F6 to stop & save");
            }
        }

        if (input->IsKeyReleased(Key::F6))
        {
            logger.StopRecording();
            logger.SaveToFile("replay_01.rep");

            logger.DumpReplaySummary();
        }

        InputInjector& injector = GetInputManager().GetInjector();

        if (input->IsKeyReleased(Key::F7))
        {
            if (injector.LoadFromFile("replay_01.rep"))
            {
                injector.DumpReplaySummary();
                ENGINE_INFO("--- REPLAY PLAYBACK: STARTED ---");
                injector.Play();
            }
            else
            {
                ENGINE_ERROR("Failed to start replay: File not found.");
            }
        }
    }

    void Application::RenderAndCleanup(RendererBase* renderer, const Color& bgColor)
    {
        if (debugMode) rootScene->DebugDraw(renderer);

        rootScene->Draw(renderer);

        if (showDebugNode) debugNode->Draw(renderer);

        RenderFrame(renderer, bgColor);

        if (isSceneDirty)
        {
            rootScene->CleanUp();
            isSceneDirty = false;
        }
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

            if (isScenePendingStart)
            {
                rootScene->Start();
                isScenePendingStart = false;
            }

            CalculateTime(accumulator, frameCount, fpsTimer, currentFPS, renderer);
            UpdateFixed(FIXED_TIMESTEP, accumulator);
            UpdateVariable();
            HandleSystemHotkeys(input);
            RenderAndCleanup(renderer, bgColor);
        }
    }

    void Application::Shutdown()
    {
        eventBus.Unsubscribe(SettingsChangedEvent::GetStaticType(), settingsEventId);
        eventBus.Unsubscribe(AudioMuteEvent::GetStaticType(), muteEventId);
        eventBus.Unsubscribe(AudioVolumeEvent::GetStaticType(), volumeEventId);

        rootScene.reset();
        debugNode.reset();

        if (audio)  audio->Shutdown();
        if (window) window->Shutdown();
        isRunning = false;
    }

    void Application::ToggleDebugMode()
    {
        debugMode = !debugMode;

        if (window && window->GetRenderer())
        {
            window->GetRenderer()->SetDebugRenderEnabled(debugMode);
        }
        ENGINE_LOG("Debug Shapes (Collisions): {}", (debugMode ? "ON" : "OFF"));
    }

    void Application::ToggleDebugNode()
    {
        showDebugNode = !showDebugNode;
        debugNode->SetActive(showDebugNode);

        ENGINE_LOG("Debug Node (FPS): {}", (showDebugNode ? "ON" : "OFF"));
    }

    void Application::RegisterCamera(CameraComponent* camera)
    {
        if (std::find(activeCameras.begin(), activeCameras.end(), camera) == activeCameras.end())
        {
            activeCameras.push_back(camera);
        }
    }

    void Application::UnregisterCamera(CameraComponent* camera)
    {
        activeCameras.erase(std::remove(activeCameras.begin(), activeCameras.end(), camera), activeCameras.end());
    }

    void Application::RegisterAudioListener(AudioListenerComponent* listener)
    {
        activeAudioListener = listener;
    }

    void Application::UnregisterAudioListener(AudioListenerComponent* listener)
    {
        if (activeAudioListener == listener)
        {
            activeAudioListener = nullptr;
        }
    }

    Node* Application::FindAudioListener() const
    {
        if (!activeAudioListener) return nullptr;
        return activeAudioListener->GetOwner()->IsActive() ? activeAudioListener->GetOwner() : nullptr;
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