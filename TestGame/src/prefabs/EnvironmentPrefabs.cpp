#include "EnvironmentPrefabs.h"

#include "scenes/Node.h"

// Components
#include "components/SpriteComponent.h"
#include "components/AnimatedSpriteComponent.h"
#include "components/ColliderComponent.h"
#include "components/RigidBodyComponent.h"
#include "components/TriggerAreaComponent.h"
#include "components/ScriptComponent.h"

// Scripts
#include "../scripts/PlatformScript.h"
#include "../scripts/CoinScript.h"
#include "../scripts/EnemyScript.h"
#include "../scripts/FinishLineScript.h"

Engine::Node* EnvironmentPrefabs::CreatePlatform(Engine::SceneBuilder& builder, const std::string& name, Engine::Texture2D tex, Engine::Vector2f pos, Engine::Vector2f scale)
{
    Engine::Node* node = builder.CreateNode(name);
    node->transform.SetPosition(pos);
    node->transform.SetScale(scale);

    node->AddComponent<Engine::SpriteComponent>(tex, Engine::Pivot::Center);
    node->AddComponent<Engine::ColliderComponent>(Engine::RectangleShape{}, Engine::Vector2f(0.0f, 0.0f), true);

    auto* rb = node->AddComponent<Engine::RigidBodyComponent>(Engine::BodyType::Static, false);
    rb->SetMass(1000.0f);
    rb->SetFriction(0.8f);
    rb->SetInertia(800.0f);
    rb->SetRestitution(0.0f);

    node->AddComponent<Engine::ScriptComponent>(new PlatformScript(pos));

    return node;
}

void EnvironmentPrefabs::CreateCoin(Engine::SceneBuilder& builder, Engine::Texture2D tex, Engine::Vector2f pos)
{
    static int id = 0;
    Engine::Node* node = builder.CreateNode("Coin_" + std::to_string(id++));
    node->transform.SetPosition(pos);
    node->transform.SetScale(Engine::Vector2(0.35f, 0.35f ));

    auto* anim = node->AddComponent<Engine::AnimatedSpriteComponent>(tex, Engine::Pivot::Center);
    anim->AddAnimationGrid("Rotate", 0, 0, 6, 200.0f, 256.0f, 0.3f, true);
    anim->Play("Rotate");

    node->AddComponent<Engine::TriggerAreaComponent>(Engine::CircleShape{}, Engine::Vector2f(0.0f, 0.0f), true);
    node->AddComponent<Engine::ScriptComponent>(new CoinScript());
}

Engine::Node* EnvironmentPrefabs::CreateBox(Engine::SceneBuilder& builder, Engine::Texture2D tex, Engine::Vector2f pos)
{
    Engine::Node* node = builder.CreateNode("Box");
    node->transform.SetPosition(pos);
    node->transform.SetScale(Engine::Vector2(0.35f, 0.35f ));

    node->AddComponent<Engine::SpriteComponent>(tex, Engine::Pivot::Center);
    node->AddComponent<Engine::ColliderComponent>(Engine::RectangleShape{}, Engine::Vector2f(0.0f, 0.0f), true);

    float realWidth = tex.size.x * 0.35f;
    float realHeight = tex.size.y * 0.35f;

    auto* rb = node->AddComponent<Engine::RigidBodyComponent>(Engine::BodyType::Dynamic, true);
    rb->SetFriction(0.8f);
    rb->SetMassAndBoxInertia(3.0f, { realWidth, realHeight });

    return node;
}

Engine::Node* EnvironmentPrefabs::CreateEnemy(Engine::SceneBuilder& builder, Engine::Texture2D tex, Engine::Vector2f pos, float leftBound, float rightBound)
{
    Engine::Node* node = builder.CreateNode("Enemy");
    node->transform.SetPosition(pos);

    // Tint red to distinguish it
    node->AddComponent<Engine::SpriteComponent>(tex, Engine::Pivot::Center, Engine::Color{ 220, 50, 50, 255 });
    node->AddComponent<Engine::ColliderComponent>(Engine::RectangleShape{}, Engine::Vector2f(0.0f, 0.0f), true);
    node->AddComponent<Engine::TriggerAreaComponent>(Engine::RectangleShape{}, Engine::Vector2f(0.0f, 0.0f), true);

    auto* rb = node->AddComponent<Engine::RigidBodyComponent>(Engine::BodyType::Dynamic);
    rb->SetLockRotation(true);
    rb->SetGravityScale(1.0f);

    node->AddComponent<Engine::ScriptComponent>(new EnemyScript(leftBound, rightBound, 120.0f));

    return node;
}

Engine::Node* EnvironmentPrefabs::CreateFinishLine(Engine::SceneBuilder& builder, Engine::Texture2D tex, Engine::Vector2f pos)
{
    Engine::Node* node = builder.CreateNode("FinishLine");
    node->transform.SetPosition(pos);
    node->transform.SetScale(Engine::Vector2(0.5f, 4.0f ));

    // Tint green
    node->AddComponent<Engine::SpriteComponent>(tex, Engine::Pivot::Center, Engine::Color{ 50, 220, 80, 200 });
    node->AddComponent<Engine::TriggerAreaComponent>(Engine::RectangleShape{}, Engine::Vector2f(0.0f, 0.0f), true);

    node->AddComponent<Engine::ScriptComponent>(new FinishLineScript());

    return node;
}