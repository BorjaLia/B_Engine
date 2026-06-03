#include "Node.h"

#include <algorithm>
#include "../core/Application.h"
#include "../math/MathDebugger.h"
#include "../debug/Debug.h"

namespace Engine
{
    Node::Node(const std::string& name)
        : name(name), parent(nullptr), transform()
    {
        transform.SetOwner(this);
    }

    Node::~Node()
    {
        // 1. Free all components to their respective pools
        for (auto& comp : components)
        {
            comp.freeFunction(comp.instance);
        }
        components.clear();

        // 2. Free all children
        for (Node* child : children)
        {
            Application::Get().GetNodePool().Free(child);
        }
        children.clear();
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
        // 1. Clean Components using Type-Erased Deleter
        for (auto it = components.begin(); it != components.end(); )
        {
            if (it->instance->IsPendingDestruction())
            {
                it->freeFunction(it->instance); // Return to pool
                it = components.erase(it);      // Remove from vector
            }
            else
            {
                ++it;
            }
        }

        // 2. Free Children memory BEFORE erasing them from the vector
        for (Node* child : children)
        {
            if (child->IsPendingDestruction())
            {
                Application::Get().GetNodePool().Free(child);
            }
        }

        // 3. Remove them from the active vector
        children.erase(
            std::remove_if(children.begin(), children.end(),
                [](Node* child)
                {
                    return child->IsPendingDestruction();
                }),
            children.end()
        );

        // 4. Propagate cleanup to survivors
        for (Node* child : children)
        {
            child->CleanUp();
        }
    }

    void Node::ClearChildren()
    {
        // Free memory for non-persistent nodes via the Pool
        for (Node* child : children)
        {
            if (!child->IsPersistent())
            {
                Application::Get().GetNodePool().Free(child);
            }
        }

        // Remove from the hierarchy
        children.erase(
            std::remove_if(children.begin(), children.end(),
                [](Node* child)
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
            if (comp.instance->IsActive() && !comp.instance->HasStarted())
            {
                comp.instance->Start();
                comp.instance->SetStarted(true);
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
        transform.UpdateTransform();

        for (auto& comp : components)
        {
            if (comp.instance->IsActive())
            {
                comp.instance->Update(deltaTime);
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
            if (comp.instance->IsActive())
            {
                comp.instance->FixedUpdate(fixedDeltaTime);
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
            if (comp.instance->IsActive())
            {
                comp.instance->Draw(renderer);
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

        //TODO transform debug draw reimplementation

        for (auto& component : components)
        {
            component.instance->DebugDraw(renderer);
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
        ss << "Local Pos: " << transform.GetPosition() << " | ";
        ss << "Global Pos: " << transform.GetGlobalPosition() << "\n";

        ss << "  Children: " << children.size() << " | Components: " << components.size() << "\n";

        for (const auto& comp : components)
        {
            ss << "    -> " << comp.instance->ToString() << "\n";
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

    void Node::AddChild(Node* child)
    {
        child->parent = this;
        children.push_back(child);
    }

    Node* Node::FindChild(std::string_view targetName) const
    {
        // 1. Direct children search
        for (Node* child : children)
        {
            if (child->name == targetName) return child;
        }

        // 2. Recursive search
        for (Node* child : children)
        {
            Node* found = child->FindChild(targetName);
            if (found != nullptr) return found;
        }

        return nullptr;
    }
}