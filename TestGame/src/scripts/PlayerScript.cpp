#include "PlayerScript.h"

#include <cmath> 
#include <algorithm>

#include "core/Application.h"
#include "events/EventBus.h"
#include "scenes/Node.h"
#include "components/AnimatedSpriteComponent.h"
#include "components/PhysicsPlayerMovementComponent.h"
#include "components/RigidBodyComponent.h"
#include "components/FollowComponent.h"
#include "components/FollowSequencerComponent.h"
#include "components/CameraComponent.h"
#include "core/interfaces/IInput.h"
#include "events/InputEvents.h"
#include "debug/Debug.h"
#include "utils/StringHash.h"

PlayerScript::PlayerScript(Engine::FollowComponent* camFollow, Engine::CameraComponent* cam)
    : cameraFollow(camFollow), camera(cam)
{
}

PlayerScript::~PlayerScript()
{
    Engine::Application::Get().GetEventBus().Unsubscribe(
        Engine::KeyReleasedEvent::GetStaticType(),
        keyReleasedSubId
    );
}

void PlayerScript::OnStart()
{
    if (!owner) return;
    ENGINE_LOG("Started player script");

    animator = owner->GetComponent<Engine::AnimatedSpriteComponent>();
    controller = owner->GetComponent<Engine::PhysicsPlayerController>();
    rb = owner->GetComponent<Engine::RigidBodyComponent>();

    if (cameraFollow && cameraFollow->GetOwner())
    {
        sequencer = cameraFollow->GetOwner()->GetComponent<Engine::FollowSequencerComponent>();
    }

    auto& bus = Engine::Application::Get().GetEventBus();
    keyReleasedSubId = bus.Subscribe<Engine::KeyReleasedEvent>(
        [this](Engine::KeyReleasedEvent& e) { this->OnKeyReleased(e); }
    );
}

void PlayerScript::OnKeyReleased(Engine::KeyReleasedEvent& e)
{
    if (e.GetKey() == Engine::Key::F && sequencer && !sequencer->IsPlaying())
    {
        Engine::Node* enemyNode = Engine::Application::Get().GetRootScene()->FindChild("Enemy");

        if (enemyNode)
        {
            ENGINE_LOG("Event-driven cinematic pan to Enemy triggered (Tick: {})", e.GetTick());

            sequencer->ClearSequence();
            sequencer->AddKeyframe({ { Engine::TrackedTarget(enemyNode, 1.0f) }, 2.5f });
            sequencer->AddKeyframe({ { Engine::TrackedTarget(owner, 1.0f) }, 0.1f });
            sequencer->Play();
        }
    }
}

void PlayerScript::OnUpdate(float deltaTime)
{
    (void)deltaTime;
    if (!animator || !rb || !controller) return; // Safety validation

    Engine::Vector2f vel = rb->GetVelocity();
    auto& input = Engine::Application::Get().GetInputManager();

    // Read the player's intention
    float moveX = input.GetAxis(Engine::Hash::GetHash("Game_MoveX"));

    // ==========================================
    // 1. HYBRID FLIP LOGIC
    // ==========================================
    if (std::abs(moveX) > 0.1f)
    {
        // Priority 1: Intention. Look where we want to go.
        animator->SetFlipX(moveX < 0);
    }
    else if (std::abs(vel.x) > 150.0f) // <-- The Magic Threshold!
    {
        // Priority 2: Extreme physics.
        // If no keys are pressed but we're moving incredibly fast (a strong external push),
        // look in the direction of the momentum.
        // The 150.0f threshold completely ignores box collision "jitter".
        animator->SetFlipX(vel.x < 0);
    }

    // ==========================================
    // 2. ANIMATION STATE MACHINE
    // ==========================================
    bool isTryingToMove = std::abs(moveX) > 0.1f;

    if (controller->GetGrounded())
    {
        if (isTryingToMove)
        {
            // Player WANTS to move
            if (controller->IsRunning())
            {
                animator->Play("Run");
                animator->speedMultiplier = std::abs(vel.x) / controller->GetWalkSpeed();
            }
            else
            {
                animator->Play("Walk");
                // Modulate animation playback speed to match actual physics speed
                animator->speedMultiplier = std::abs(vel.x) / controller->GetWalkSpeed();
            }

            // AAA Trick: If the player pushes a wall, vel.x is 0. 
            // To prevent a frozen "Moonwalk", we force a minimum animation speed.
            if (animator->speedMultiplier < 0.3f) animator->speedMultiplier = 0.3f;
        }
        else
        {
            // No input: Force Idle regardless of tiny physics impulses from boxes
            animator->Play("Idle");
            animator->speedMultiplier = 1.0f;
        }
    }
    else
    {
        // Mid-air
        if (animator->GetCurrentAnimation() != "Jump")
        {
            animator->Play("Jump");
            animator->speedMultiplier = 1.0f;
        }
    }

    // ==========================================
    // 3. CAMERA LOGIC: MOUSE PEEKING
    // ==========================================
    if (cameraFollow && camera)
    {
        if (sequencer && sequencer->IsPlaying()) return;

        Engine::Vector2f playerPos = owner->GetGlobalPosition();
        auto& inputManager = Engine::Application::Get().GetInputManager();
        Engine::Vector2f mouseScreenPos = {
            inputManager.GetAxis(Engine::Hash::GetHash("Pointer_X")),
            inputManager.GetAxis(Engine::Hash::GetHash("Pointer_Y"))
        };
        Engine::Vector2f mouseWorldPos = camera->ScreenToWorld(mouseScreenPos);

        Engine::Vector2f diff = mouseWorldPos - playerPos;
        float dist = std::sqrt(diff.x * diff.x + diff.y * diff.y);

        // --- DESIGN PARAMETERS ---
        const float peekThreshold = 150.0f;
        const float maxPeekDistance = 600.0f;
        const float maxPeekWeight = 0.6f;
        const float peekSensitivity = 0.005f;

        // BASE DEADZONE CONFIGURATION
        const Engine::Vector2f baseDeadzone = { 150.0f, 150.0f };
        const float baseCenteringSpeed = 100.0f;

        std::vector<Engine::TrackedTarget> currentTargets;
        currentTargets.push_back(Engine::TrackedTarget(owner, 1.0f));

        float currentPeekWeight = 0.0f; // Tracking how much we are peeking

        if (dist > peekThreshold)
        {
            float clampedDist = std::min(dist, maxPeekDistance);
            float activeDist = clampedDist - peekThreshold;

            Engine::Vector2f dir = diff / dist;
            Engine::Vector2f effectiveMousePos = playerPos + (dir * activeDist);

            currentPeekWeight = std::min(activeDist * peekSensitivity, maxPeekWeight);

            currentTargets.push_back(Engine::TrackedTarget(effectiveMousePos, currentPeekWeight));
        }

        cameraFollow->SetTargets(currentTargets);

        // ==========================================
        // DYNAMIC DEADZONE MAGIC!
        // ==========================================
        // 1. Calculate peek ratio (0.0 to 1.0)
        float peekRatio = currentPeekWeight / maxPeekWeight;

        // 2. Smoothly shrink the bounding box. If peekRatio is 1 (max peek), box is 0.
        Engine::Vector2f dynamicDeadzone = {
            baseDeadzone.x * (1.0f - peekRatio),
            baseDeadzone.y * (1.0f - peekRatio)
        };

        // 3. Inject the new deadzone frame-by-frame
        cameraFollow->SetDeadzone(dynamicDeadzone, true, baseCenteringSpeed);
    }

    // ==========================================
    // 4. ZOOM LOGIC (Mouse Wheel)
    // ==========================================
    // 1. Read absolute axis position from the wheel
    float currentScroll = Engine::Application::Get().GetInputManager().GetAxis(Engine::Hash::GetHash("Pointer_Scroll"));

    // 2. Calculate true delta
    float scrollDelta = currentScroll - lastScroll;
    lastScroll = currentScroll; // Update memory

    if (std::abs(scrollDelta) > 0.001f)
    {
        if (camera)
        {
            // 0.1f controls zoom sensitivity
            float newZoom = camera->GetZoom() + (scrollDelta * 0.1f);

            // Clamp zoom to prevent infinite scaling or inverting the camera
            newZoom = std::clamp(newZoom, 0.2f, 2.0f);

            camera->SetZoom(newZoom);
        }
    }
}