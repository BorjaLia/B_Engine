#pragma once

namespace Engine
{
    class Node;

    /// The base class for all user-defined scripts (similar to Unity's MonoBehaviour).
    /// Users inherit from this class to write custom game logic.
    class Script
    {
    public:
        virtual ~Script() = default;

        /// Called once when the script is first initialized in the scene.
        virtual void OnStart() {}

        /// Called every frame. Ideal for input polling and visual updates.
        /// @param deltaTime Time elapsed since the last frame in seconds.
        virtual void OnUpdate(float deltaTime) { (void)deltaTime; }

        /// Called at a fixed time interval. Ideal for physics manipulations.
        /// @param fixedDeltaTime The fixed time step in seconds.
        virtual void OnFixedUpdate(float fixedDeltaTime) { (void)fixedDeltaTime; }

        /// Called right before the script or its owner node is destroyed.
        virtual void OnDestroy() {}

        /// Injects the node dependency. Used internally by the engine.
        void SetOwner(Node* node) { owner = node; }

        /// Retrieves the node that owns this script.
        Node* GetOwner() const { return owner; }

    protected:
        /// Pointer to the node that owns this script. 
        /// Protected so derived user scripts can access it freely.
        Node* owner = nullptr;
    };
}