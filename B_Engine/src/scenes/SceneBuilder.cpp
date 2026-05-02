#include "SceneBuilder.h"

#include "Node.h"
#include "../core/Application.h"
#include "../debug/Debug.h"

namespace Engine
{
    Node* SceneBuilder::CreateNode(const std::string& name)
    {
        auto node = std::make_unique<Node>(name);
        Node* raw = node.get();
        pendingNodes.push_back({ std::move(node), nullptr });
        return raw;
    }

    Node* SceneBuilder::CreateChildNode(Node* parent, const std::string& name)
    {
        // The child is owned by its parent node, not by pendingNodes directly.
        auto child = std::make_unique<Node>(name);
        Node* raw = child.get();
        pendingNodes.push_back({ std::move(child), parent });
        return raw;
    }

    Node* SceneBuilder::FindPending(const std::string& name)
    {
        for (auto& pending : pendingNodes)
        {
            Node* current = pending.node.get();
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
                rootScene->AddChild(std::move(pending.node));
            }
            else
            {
                pending.parent->AddChild(std::move(pending.node));
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