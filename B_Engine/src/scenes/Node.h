#pragma once

#include <string>
#include <vector>
#include <memory>
#include <sstream>
#include <algorithm>
#include <string_view>
#include <type_traits>

#include "../utils/Types.h"
#include "../components/Component.h"
#include "../memory/ComponentRegistry.h"
#include "Transform.h"
#include "../components/CameraComponent.h"
#include "../components/ColliderComponent.h"
#include "../components/TriggerAreaComponent.h"
#include "../debug/Debug.h"

namespace Engine
{
    class RendererBase;

    /// Wrapper to hold a raw component pointer and its type-erased memory pool releaser.
    struct ComponentRecord
    {
        Component* instance = nullptr;
        void (*freeFunction)(Component*) = nullptr;
    };

    /// @defgroup Scenes Scene Management
    /// @brief The Node graph, Scene Builder, and scene transitions.
    ///
    /// Represents an entity in the scene graph.
    /// Handles component lifecycle, parent-child hierarchy, and persistence.
    /// @ingroup Scenes
    class Node
    {
    public:
        std::string name;

        Transform transform;

        /// Constructs a node with an optional name.
        /// @param name The name identifier for the node.
        Node(const std::string& name = "Node");
        
        /// Destructor recursively frees children to the NodePool
        virtual ~Node();

#pragma region Lifecycle & State
        bool IsActive() const { return isActive; }
        void SetActive(bool active) { isActive = active; }

        bool IsPendingDestruction() const { return pendingDestruction; }

        /// Marks the node and its components for destruction at the end of the frame.
        virtual void Destroy();

        /// Physically removes destroyed components and children from memory.
        virtual void CleanUp();

        /// Clears all children, EXCEPT those marked as persistent.
        void ClearChildren();

        /// Marks a node to survive scene transitions (DontDestroyOnLoad).
        /// @param persistent True to keep the node alive during scene loads.
        void SetPersistent(bool persistent);
        bool IsPersistent() const { return isPersistent; }
#pragma endregion

#pragma region Core Loop
        void Start();
        void Update(float deltaTime);
        void FixedUpdate(float fixedDeltaTime);
        void Draw(RendererBase* renderer);
        void DebugDraw(RendererBase* renderer);
#pragma endregion

#pragma region Hierarchy & Position
        void AddChild(Node* child);
        Node* FindChild(std::string_view targetName) const;
        Node* GetParent() const { return parent; }
        const std::vector<Node*>& GetChildren() const { return children; }
#pragma endregion

#pragma region Component Management
        /// Instantiates and attaches a component of type T to this node utilizing Data-Oriented Pools.
        template <typename T, typename... Args>
        T* AddComponent(Args&&... args)
        {
            // 1. Allocate from the specific contiguous pool for this Component Type
            T* rawPtr = ComponentRegistry::GetPool<T>().Allocate(std::forward<Args>(args)...);
            rawPtr->SetOwner(this);
            rawPtr->Initialize();

            // 2. Create the Type-Erased Deleter
            auto freeFunc = [](Component* comp)
                {
                    // Safely cast back to T* and return it to its specific memory pool
                    ComponentRegistry::GetPool<T>().Free(static_cast<T*>(comp));
                };

            // 3. Store the record
            components.push_back({ rawPtr, freeFunc });
            NotifyEnginePendingStart();

            return rawPtr;
        }

        /// Retrieves the first attached component of type T, or nullptr if none exists.
        template <typename T>
        T* GetComponent()
        {
            for (auto& comp : components)
            {
                T* target = dynamic_cast<T*>(comp.instance);
                if (target != nullptr)
                {
                    return target;
                }
            }
            return nullptr;
        }

        const std::vector<ComponentRecord>& GetAllComponents() const { return components; }
#pragma endregion

#pragma region Debugging
        std::string ToString() const;
        void DumpTree(int indentLevel = 0) const;
#pragma endregion

    protected:
        void NotifyEnginePendingStart();

    private:
        bool isActive = true;
        bool pendingDestruction = false;
        bool isPersistent = false;

        std::vector<ComponentRecord> components;
        std::vector<Node*> children;
        Node* parent = nullptr;
    };
}