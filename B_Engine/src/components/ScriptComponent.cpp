#include "ScriptComponent.h"

namespace Engine
{
    ScriptComponent::ScriptComponent(Script* userScript)
        : script(userScript)
    {
    }

    ScriptComponent::~ScriptComponent()
    {
        if (script) script->OnDestroy();
    }

    void ScriptComponent::Initialize()
    {
        // Since AddComponent already set our 'owner', we pass it to the user script.
        if (script && owner)
        {
            script->SetOwner(owner);
        }
    }

    void ScriptComponent::Start()
    {
        if (script) script->OnStart();
    }

    void ScriptComponent::Update(float deltaTime)
    {
        if (script) script->OnUpdate(deltaTime);
    }

    void ScriptComponent::FixedUpdate(float fixedDeltaTime)
    {
        if (script) script->OnFixedUpdate(fixedDeltaTime);
    }
}