#pragma once

#include <string>
#include <vector>
#include <memory>
#include <sstream>
#include <algorithm>
#include <string_view>

#include "../utils/Types.h"
#include "../components/Component.h"
#include "../components/TransformComponent.h"
#include "../components/CameraComponent.h"
#include "../components/ColliderComponent.h"
#include "../components/TriggerAreaComponent.h"
#include "../debug/Debug.h"

namespace Engine
{
    class IRenderer;

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
        TransformComponent* transform;

        /// Constructs a node with an optional name.
        /// @param name The name identifier for the node.
        Node(const std::string& name = "Node");
        virtual ~Node() = default;

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
        void Draw(IRenderer* renderer);
        void DebugDraw(IRenderer* renderer);
#pragma endregion

#pragma region Hierarchy & Position
        void AddChild(std::unique_ptr<Node> child);
        Node* FindChild(std::string_view targetName) const;
        Node* GetParent() const { return parent; }
        const std::vector<std::unique_ptr<Node>>& GetChildren() const { return children; }

        Vector2f GetGlobalPosition() const;
#pragma endregion

#pragma region Component Management
        /// Instantiates and attaches a component of type T to this node.
        template <typename T, typename... Args>
        T* AddComponent(Args&&... args)
        {
            auto newComponent = std::make_unique<T>(std::forward<Args>(args)...);
            newComponent->SetOwner(this);

            T* rawPtr = newComponent.get();
            components.push_back(std::move(newComponent));

            rawPtr->Initialize();
            NotifyEnginePendingStart();

            return rawPtr;
        }

        /// Retrieves the first attached component of type T, or nullptr if none exists.
        template <typename T>
        T* GetComponent()
        {
            for (auto& comp : components)
            {
                T* target = dynamic_cast<T*>(comp.get());
                if (target != nullptr)
                {
                    return target;
                }
            }
            return nullptr;
        }

        void GetAllCameras(std::vector<CameraComponent*>& outCameras);
        void GetAllColliders(std::vector<ColliderComponent*>& outColliders);
        void GetAllTriggerAreas(std::vector<TriggerAreaComponent*>& outTriggerAreas);
        const std::vector<std::unique_ptr<Component>>& GetAllComponents() const { return components; }
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

        std::vector<std::unique_ptr<Component>> components;
        std::vector<std::unique_ptr<Node>> children;
        Node* parent = nullptr;
    };
}