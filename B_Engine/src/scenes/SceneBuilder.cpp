#include "SceneBuilder.h"

#include "Node.h"
#include "../core/Application.h"
#include "../debug/Debug.h"

namespace Engine
{
    Node* SceneBuilder::CreateNode(const std::string& name)
    {
        Node* raw = Application::Get().GetNodePool().Allocate(name);
        pendingNodes.push_back({ raw, nullptr });
        return raw;
    }

    Node* SceneBuilder::CreateChildNode(Node* parent, const std::string& name)
    {
        // The child is owned by its parent node, not by pendingNodes directly.
        Node* raw = Application::Get().GetNodePool().Allocate(name);
        pendingNodes.push_back({ raw, parent });
        return raw;
    }

    Node* SceneBuilder::FindPending(const std::string& name)
    {
        for (auto& pending : pendingNodes)
        {
            Node* current = pending.node;
            if (current->name == name) return current;

            Node* found = current->FindChild(name);
            if (found) return found;
        }

        ENGINE_WARN("Pending node '{}' not found!", name);
        return nullptr;
    }

    void SceneBuilder::FlushToScene(Node* rootScene)
    {
        if (pendingNodes.empty()) return;

        bool nodesAdded = false;

        for (auto& pending : pendingNodes)
        {
            if (pending.parent == nullptr)
            {
                rootScene->AddChild(pending.node);
            }
            else
            {
                pending.parent->AddChild(pending.node);
            }
            nodesAdded = true;
        }

        // Clear the waitlist
        pendingNodes.clear();

        // Notify the app to call Start() on new nodes in the next cycle
        if (nodesAdded)
        {
            Application::Get().MarkScenePendingStart();
        }
    }
}