#include "MouseScript.h"

#include <string>

#include "scenes/Node.h"
#include "debug/Debug.h"
#include "core/Application.h"
#include "components/CameraComponent.h"
#include "components/SpriteComponent.h"
#include "components/RigidBodyComponent.h"
#include "components/ColliderComponent.h"
#include "events/EventBus.h"
#include "events/InputEvents.h"
#include "utils/StringHash.h"

namespace
{
    int mouseBlockId = 0;
}

MouseScript::MouseScript(Engine::CameraComponent* camera)
    : camera(camera)
{
}

MouseScript::~MouseScript()
{
    Engine::Application::Get().GetEventBus().Unsubscribe(
        Engine::ActionChangedEvent::GetStaticType(),
        actionSubId
    );
}

void MouseScript::OnStart()
{
    if (!owner) return;
    ENGINE_LOG("Started mouse script");

    auto& bus = Engine::Application::Get().GetEventBus();
    actionSubId = bus.Subscribe<Engine::ActionChangedEvent>(
        [this](Engine::ActionChangedEvent& e) { this->OnActionChanged(e); }
    );
}

void MouseScript::OnUpdate(float deltaTime)
{
    (void)deltaTime;
    if (!camera) return;

    auto& input = Engine::Application::Get().GetInputManager();

    // Read from the VIRTUAL INPUT SYSTEM
    Engine::Vector2f screenMousePos = {
        input.GetAxis(Engine::Hash::GetHash("Pointer_X")),
        input.GetAxis(Engine::Hash::GetHash("Pointer_Y"))
    };

    Engine::Vector2f worldMousePos = camera->ScreenToWorld(screenMousePos);
    owner->transform->SetPosition(worldMousePos);
}

void MouseScript::OnActionChanged(Engine::ActionChangedEvent& e)
{
    // If the UI consumed the event, we back off.
    if (e.handled) return;

    // We only care about presses (Value > 0), ignore when the click is released.
    if (e.GetValue() == 0.0f) return;

    auto& input = Engine::Application::Get().GetInputManager();
    Engine::Vector2f screenMousePos = {
        input.GetAxis(Engine::Hash::GetHash("Pointer_X")),
        input.GetAxis(Engine::Hash::GetHash("Pointer_Y"))
    };
    Engine::Vector2f worldMousePos = camera->ScreenToWorld(screenMousePos);

    if (e.GetHash() == Engine::Hash::GetHash("Game_SpawnRight"))
    {
        e.handled = true;
        Engine::Node* node = Engine::Application::Get().GetSceneBuilder().CreateNode("block " + std::to_string(mouseBlockId++));

        Engine::Texture2D tex = Engine::Application::Get().GetResourceManager()->GetTexture("res/sprites/NoTexture.png");

        node->AddComponent<Engine::SpriteComponent>(tex, Engine::Pivot::Center);
        node->AddComponent<Engine::ColliderComponent>(Engine::CircleShape{}, Engine::Vector2f(0.0f, 0.0f), true);

        float realWidth = tex.size.x * 1.0f;
        float realHeight = tex.size.y * 1.0f;

        auto* rb = node->AddComponent<Engine::RigidBodyComponent>(Engine::BodyType::Dynamic, false);
        rb->SetFriction(100.0f);
        rb->SetMassAndBoxInertia(0.0f, { realWidth, realHeight });
        rb->SetGravityScale(0.0f);

        node->transform->SetPosition(worldMousePos);

        Engine::Application::Get().GetTimerManager().SetTimeout(10.0f, [node]
            {
                ENGINE_INFO("Time out!");
                node->Destroy();
            });
    }
    else if (e.GetHash() == Engine::Hash::GetHash("UI_Interact"))
    {
        Engine::Node* node = Engine::Application::Get().GetSceneBuilder().CreateNode("block " + std::to_string(mouseBlockId++));

        Engine::Texture2D tex = Engine::Application::Get().GetResourceManager()->GetTexture("res/sprites/NoTexture.png");

        node->AddComponent<Engine::SpriteComponent>(tex, Engine::Pivot::Center);
        node->AddComponent<Engine::ColliderComponent>(Engine::RectangleShape{}, Engine::Vector2f(0.0f, 0.0f), true);

        float realWidth = tex.size.x * 1.0f;
        float realHeight = tex.size.y * 1.0f;

        auto* rb = node->AddComponent<Engine::RigidBodyComponent>(Engine::BodyType::Dynamic, false);
        rb->SetFriction(0.8f);
        rb->SetMassAndBoxInertia(5.0f, { realWidth, realHeight });

        node->transform->SetPosition(worldMousePos);

        Engine::Application::Get().GetTimerManager().SetTimeout(10.0f, [node]
            {
                ENGINE_INFO("Time out!");
                node->Destroy();
            });
    }
}