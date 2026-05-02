#include "Component.h"

#include "../core/Application.h" 
#include "../debug/Debug.h"

namespace Engine
{
    void Component::Destroy()
    {
        pendingDestruction = true;
        isActive = false; // Turn it off instantly

        Application::Get().MarkSceneDirty();

        if (!Application::Get().IsDebugMode()) { return; }

        ENGINE_LOG("DELETED COMPONENT");
    }
}