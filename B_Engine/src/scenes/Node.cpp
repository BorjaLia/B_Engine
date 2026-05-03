#include "Node.h"

#include <algorithm>
#include "../core/Application.h"
#include "../debug/Debug.h"

namespace Engine
{
    Node::Node(const std::string& name) : name(name), parent(nullptr)
    {
        // 1. Mandatory instantiation of TransformComponent
        auto newTransform = std::make_unique<TransformComponent>();
        transform = newTransform.get();

        // 2. Add it to the normal list for Update() and Draw()
        newTransform->SetOwner(this);
        components.push_back(std::move(newTransform));
    }

    void Node::NotifyEnginePendingStart()
    {
        Application::Get().MarkScenePendingStart();
    }

    void Node::SetPersistent(bool persistent)
    {
        isPersistent = persistent;

        // Safety check: Persistence only guarantees survival if attached to the Root node.
        if (isPersistent && parent != nullptr && parent->name != "Root")
        {
            ENGINE_WARN("Node '{}' set to persistent but is not a root child. It will die if its parent is destroyed.", name);
        }
    }

    void Node::Destroy()
    {
        pendingDestruction = true;
        isActive = false;

        Application::Get().MarkSceneDirty();

        if (Application::Get().IsDebugMode())
        {
            ENGINE_LOG("DELETED: {}", name);
        }
    }

    void Node::CleanUp()
    {
        // 1. Clean Components
        components.erase(
            std::remove_if(components.begin(), components.end(),
                [](const std::unique_ptr<Component>& comp)
                {
                    return comp->IsPendingDestruction();
                }),
            components.end()
        );

        // 2. Clean Children
        children.erase(
            std::remove_if(children.begin(), children.end(),
                [](const std::unique_ptr<Node>& child)
                {
                    return child->IsPendingDestruction();
                }),
            children.end()
        );

        // 3. Propagate cleanup to survivors
        for (auto& child : children)
        {
            child->CleanUp();
        }
    }

    void Node::ClearChildren()
    {
        // Remove children EXCEPT the ones marked as persistent
        children.erase(
            std::remove_if(children.begin(), children.end(),
                [](const std::unique_ptr<Node>& child)
                {
                    return !child->IsPersistent();
                }),
            children.end()
        );
    }

    void Node::Start()
    {
        if (!isActive) return;

        // 1. Start newborn components
        for (auto& comp : components)
        {
            if (comp->IsActive() && !comp->HasStarted())
            {
                comp->Start();
                comp->SetStarted(true);
            }
        }

        // 2. Propagate to children
        for (auto& child : children)
        {
            child->Start();
        }
    }

    void Node::Update(float deltaTime)
    {
        if (!isActive) return;

        // Force transform update before logic
        transform->UpdateTransform();

        for (auto& comp : components)
        {
            // Skip transform as it was manually updated above
            if (comp.get() != transform && comp->IsActive())
            {
                comp->Update(deltaTime);
            }
        }

        for (auto& child : children)
        {
            child->Update(deltaTime);
        }
    }

    void Node::FixedUpdate(float fixedDeltaTime)
    {
        if (!isActive) return;

        for (auto& comp : components)
        {
            if (comp->IsActive() && comp.get() != transform)
            {
                comp->FixedUpdate(fixedDeltaTime);
            }
        }

        for (auto& child : children)
        {
            child->FixedUpdate(fixedDeltaTime);
        }
    }

    void Node::Draw(RendererBase* renderer)
    {
        if (!isActive) return;

        for (auto& comp : components)
        {
            if (comp->IsActive())
            {
                comp->Draw(renderer);
            }
        }

        for (auto& child : children)
        {
            child->Draw(renderer);
        }
    }

    void Node::DebugDraw(RendererBase* renderer)
    {
        if (!isActive) return;

        for (auto& component : components)
        {
            component->DebugDraw(renderer);
        }

        for (auto& child : children)
        {
            child->DebugDraw(renderer);
        }
    }

    std::string Node::ToString() const
    {
        std::stringstream ss;
        ss << "Node: '" << name << "' | ";
        ss << "Local Pos: " << transform->GetPosition() << " | ";
        ss << "Global Pos: " << GetGlobalPosition() << "\n";

        ss << "  Children: " << children.size() << " | Components: " << components.size() << "\n";

        for (const auto& comp : components)
        {
            ss << "    -> " << comp->ToString() << "\n";
        }
        return ss.str();
    }

    void Node::DumpTree(int indentLevel) const
    {
        std::string indent(indentLevel * 2, ' ');
        ENGINE_LOG("{}-> {}", indent, ToString());

        for (const auto& child : children)
        {
            child->DumpTree(indentLevel + 1);
        }
    }

    void Node::AddChild(std::unique_ptr<Node> child)
    {
        child->parent = this;
        children.push_back(std::move(child));
    }

    Node* Node::FindChild(std::string_view targetName) const
    {
        // 1. Direct children search
        for (const auto& child : children)
        {
            if (child->name == targetName)
            {
                return child.get();
            }
        }

        // 2. Recursive search
        for (const auto& child : children)
        {
            Node* found = child->FindChild(targetName);
            if (found != nullptr)
            {
                return found;
            }
        }

        return nullptr;
    }

    Vector2f Node::GetGlobalPosition() const
    {
        return transform->GetGlobalPosition();
    }

    void Node::GetAllCameras(std::vector<CameraComponent*>& outCameras)
    {
        CameraComponent* cam = GetComponent<CameraComponent>();
        if (cam != nullptr)
        {
            outCameras.push_back(cam);
        }

        for (auto& child : children)
        {
            child->GetAllCameras(outCameras);
        }
    }

    void Node::GetAllColliders(std::vector<ColliderComponent*>& outColliders)
    {
        if (!isActive) return;

        if (auto* col = GetComponent<ColliderComponent>())
        {
            if (col->IsActive())
            {
                outColliders.push_back(col);
            }
        }

        for (auto& child : children)
        {
            child->GetAllColliders(outColliders);
        }
    }

    void Node::GetAllTriggerAreas(std::vector<TriggerAreaComponent*>& outTriggerAreas)
    {
        if (!isActive) return;

        if (auto* col = GetComponent<TriggerAreaComponent>())
        {
            if (col->IsActive())
            {
                outTriggerAreas.push_back(col);
            }
        }

        for (auto& child : children)
        {
            child->GetAllTriggerAreas(outTriggerAreas);
        }
    }
}