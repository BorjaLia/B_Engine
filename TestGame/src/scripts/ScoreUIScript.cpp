#include "ScoreUIScript.h"

#include <string>

#include "scenes/Node.h"
#include "core/Application.h"
#include "components/TextComponent.h"
#include "events/EventBus.h"

#include "../events/GameEvents.h"

void ScoreUIListener::OnStart()
{
    eventBus = &Engine::Application::Get().GetEventBus();

    listenerId = eventBus->Subscribe<CoinCollectedEvent>([this](CoinCollectedEvent& e)
        {
            currentCoins += e.GetValue();

            if (auto* textComp = owner->GetComponent<Engine::TextComponent>())
            {
                textComp->SetText("Coins: " + std::to_string(currentCoins));
            }
        });
}

void ScoreUIListener::OnDestroy()
{
    if (eventBus)
    {
        eventBus->Unsubscribe(CoinCollectedEvent::GetStaticType(), listenerId);
    }
}