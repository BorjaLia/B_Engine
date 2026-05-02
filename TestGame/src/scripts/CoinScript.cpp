#include "CoinScript.h"

#include "core/Application.h"
#include "scenes/Node.h"
#include "components/TriggerAreaComponent.h"
#include "events/EventBus.h"

#include "../events/GameEvents.h"

void CoinScript::OnStart()
{
    auto* trigger = owner->GetComponent<Engine::TriggerAreaComponent>();
    if (trigger)
    {
        trigger->ConnectTriggerEnter([this](Engine::Node* other)
            {
                if (other->name == "Player")
                {
                    // Create the event and broadcast it
                    CoinCollectedEvent coinEvent(1);
                    Engine::Application::Get().GetEventBus().Publish(coinEvent);

                    // Despawn the coin
                    owner->Destroy();
                }
            });
    }
}