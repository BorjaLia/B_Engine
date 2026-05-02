#pragma once

#include <cstdint>

#include "scripts/Script.h"

namespace Engine
{
    class AnimatedSpriteComponent;
    class PhysicsPlayerController;
    class RigidBodyComponent;
    class FollowComponent;
    class FollowSequencerComponent;
    class CameraComponent;
    class KeyReleasedEvent;
}

/// The core brain of the player character. 
/// Handles animation states, dynamic camera lookahead, and cinematic triggers.
class PlayerScript : public Engine::Script
{
public:
    PlayerScript(Engine::FollowComponent* camFollow = nullptr, Engine::CameraComponent* cam = nullptr);
    ~PlayerScript() override;

    void OnStart() override;
    void OnUpdate(float deltaTime) override;

    void OnKeyReleased(Engine::KeyReleasedEvent& e);

private:
    Engine::AnimatedSpriteComponent* animator = nullptr;
    Engine::PhysicsPlayerController* controller = nullptr;
    Engine::RigidBodyComponent* rb = nullptr;

    Engine::FollowComponent* cameraFollow = nullptr;
    Engine::FollowSequencerComponent* sequencer = nullptr;
    Engine::CameraComponent* camera = nullptr;

    uint32_t keyReleasedSubId = 0;

    float lastScroll = 0.0f;
};