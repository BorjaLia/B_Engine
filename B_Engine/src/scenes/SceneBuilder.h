#pragma once

#include <memory>
#include <vector>
#include <string>

#include "../utils/Types.h"
#include "Node.h"

namespace Engine
{
    class Application;

    /// Fluent helper for constructing scenes before they are active.
    /// Call CreateNode() to get a raw pointer you can configure immediately.
    /// All created nodes are flushed into the root scene by Application::Run().
    /// @ingroup Scenes
    class SceneBuilder
    {
    public:
        SceneBuilder(const SceneBuilder&) = delete;
        SceneBuilder& operator=(const SceneBuilder&) = delete;
        SceneBuilder(SceneBuilder&&) = delete;
        SceneBuilder& operator=(SceneBuilder&&) = delete;

        /// Creates a new node, retains ownership, and returns a raw pointer for configuration.
        /// @param name The name of the new node.
        Node* CreateNode(const std::string& name = "Node");

        /// Creates a node that is a child of an already-created node.
        /// @param parent Must be a pointer previously returned by CreateNode().
        /// @param name The name of the child node.
        Node* CreateChildNode(Node* parent, const std::string& name = "Node");

        /// Finds a node currently held in the builder queue.
        Node* FindPending(const std::string& name);

        /// Moves all top-level nodes into the root scene. 
        /// Called automatically by the engine; do not call manually.
        void FlushToScene(Node* rootScene);

    private:
        // Restrict creation to the Application core.
        friend class Application;

        SceneBuilder() = default;
        ~SceneBuilder() = default;

        struct PendingNode
        {
            std::unique_ptr<Node> node;
            Node* parent; // If nullptr, belongs to RootScene
        };

        std::vector<PendingNode> pendingNodes;
    };
}