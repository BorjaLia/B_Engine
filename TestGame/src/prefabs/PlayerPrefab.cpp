#include "PlayerPrefab.h"

#include "scenes/Node.h"
#include "core/Application.h"

// Components
#include "components/AnimatedSpriteComponent.h"
#include "components/RigidBodyComponent.h"
#include "components/ColliderComponent.h"
#include "components/PhysicsPlayerMovementComponent.h"
#include "components/CameraComponent.h"
#include "components/FollowComponent.h"
#include "components/FollowSequencerComponent.h"
#include "components/TextComponent.h"
#include "components/AudioListenerComponent.h"
#include "components/DebugTrackingComponent.h"
#include "components/ScriptComponent.h"

// Scripts
#include "../scripts/PlayerScript.h"

Engine::Node* PlayerPrefab::Create(Engine::SceneBuilder& builder, Engine::Vector2f position)
{
    auto& rm = *Engine::Application::Get().GetResourceManager();

    Engine::Texture2D animPlayer = rm.GetTexture("res/sprites/CharacterSheet.png");
    Engine::Font* font = rm.GetFont("res/fonts/ReemKufiFunRegular.ttf", 48);

    // 1. Create Base Node
    Engine::Node* node = builder.CreateNode("Player");
    node->transform.SetPosition(position);
    node->transform.SetScale(Engine::Vector2(4.0f, 4.0f ));

    // 2. Animations
    auto* anim = node->AddComponent<Engine::AnimatedSpriteComponent>(animPlayer, Engine::Pivot::Center);
    anim->AddAnimationGrid("Walk", 0, 0, 6, 32.0f, 48.0f, 0.1f, true);
    anim->AddAnimationGrid("Run", 0, 1, 6, 32.0f, 48.0f, 0.1f, true);
    anim->AddAnimationGrid("Jump", 0, 2, 5, 32.0f, 48.0f, 0.125f, false);
    anim->AddAnimationGrid("Idle", 0, 3, 4, 32.0f, 48.0f, 0.1f, true);
    anim->AddAnimationGrid("Attack", 0, 4, 6, 48.0f, 48.0f, 0.125f, false);
    anim->Play("Idle");

    // 3. Physics & Core
    node->AddComponent<Engine::AudioListenerComponent>();
    node->AddComponent<Engine::ColliderComponent>(Engine::RectangleShape{}, Engine::Vector2f(0, 0), true);

    // Using the simplified constructor: walkSpd, runSpd, jumpFrc
    node->AddComponent<Engine::PhysicsPlayerController>(600.0f, 1200.0f, 750.0f);
    node->AddComponent<Engine::DebugTrackingComponent>();

    auto* rb = node->AddComponent<Engine::RigidBodyComponent>(Engine::BodyType::Dynamic, true);
    rb->SetFriction(0.8f);

    float realWidth = 32 * 4.0f;
    float realHeight = 48 * 4.0f;
    rb->SetMassAndBoxInertia(5.0f, { realWidth, realHeight });

    // 4. Camera System (As an independent sibling node)
    Engine::Node* camNode = builder.CreateNode("Camera");
    auto* camComp = camNode->AddComponent<Engine::CameraComponent>(0.5f);
    camNode->transform.SetPosition(position);

    auto* follow = camNode->AddComponent<Engine::FollowComponent>(Engine::FollowMode::Lerp, 5.0f);
    follow->AddTarget(Engine::TrackedTarget(node, 1.0f));
    follow->SetDeadzone({ 150.0f, 150.0f }, true, 100.0f);
    follow->SetLookahead(0.4f, 3.0f);
    follow->SetCenteringDelay(0.75f);

    camNode->AddComponent<Engine::FollowSequencerComponent>();

    // 5. Aesthetics (Tag Name)
    Engine::Node* tagNode = builder.CreateChildNode(node, "tag");
    tagNode->transform.SetPosition(Engine::Vector2f(0.0f, 25.0f));
    auto* txt = tagNode->AddComponent<Engine::TextComponent>(font, "Player", 26.0f, Engine::Color{ 255, 255, 255, 255 }, Engine::RenderLayer::World);
    txt->pivot = Engine::Pivot::Center;

    // 6. The Visual Brain (Script)
    node->AddComponent<Engine::ScriptComponent>(new PlayerScript(follow, camComp));

    return node;
}