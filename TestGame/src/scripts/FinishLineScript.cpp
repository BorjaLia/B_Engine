#include "FinishLineScript.h"

#include "scenes/Node.h"
#include "components/TriggerAreaComponent.h"
#include "GameManager.h"

void FinishLineScript::OnStart()
{
    auto* trigger = owner->GetComponent<Engine::TriggerAreaComponent>();
    if (trigger)
    {
        trigger->ConnectTriggerEnter([](Engine::Node* other)
            {
                if (other->name == "Player" && GameManager::Get())
                {
                    GameManager::Get()->Win();
                }
            });
    }
}