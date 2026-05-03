#pragma once

#include <vector>

#include "Physics.h" 

namespace Engine
{
    class Node;
    class ColliderComponent;
    class RigidBodyComponent;
    class TriggerAreaComponent;

    /// Internal structure to map a Collider with its RigidBody (O(1) access).
    struct PhysicsBody
    {
        ColliderComponent* collider = nullptr;
        RigidBodyComponent* rigidBody = nullptr;

        bool isStatic = false;
        bool needsObbUpdate = true;
    };

    /// @defgroup Physics Physics System
    /// @brief Collisions, resolving manifolds, and spatial triggers.
    ///
    /// Orchestrates the physics pipeline, spatial queries, and collision resolution.
    /// @ingroup Physics
    class PhysicsSystem
    {
    public:
        PhysicsSystem();
        ~PhysicsSystem() = default;

        /// The physical heart of the engine. Evaluates movement and collisions.
        void Update(Node* rootScene, float fixedDeltaTime);

        /// Quality of Life control.
        /// Higher numbers mean more solid physics but higher CPU cost. (Box2D uses 8 by default).
        void SetSolverIterations(int iterations) { solverIterations = iterations; }
        int GetSolverIterations() const { return solverIterations; }

        // ==========================================
        // COMPONENT REGISTRATION API
        // ==========================================
        void RegisterCollider(ColliderComponent* collider);
        void UnregisterCollider(ColliderComponent* collider);

        void RegisterTrigger(TriggerAreaComponent* trigger);
        void UnregisterTrigger(TriggerAreaComponent* trigger);

        void OnRigidBodyAdded(RigidBodyComponent* rigidBody);
        void OnRigidBodyRemoved(RigidBodyComponent* rigidBody);

        /// Forces the system to recalculate the OBB for a static collider.
        void MarkObbDirty(ColliderComponent* collider);

    private:
        int solverIterations = 8;

        // ==========================================
        // MEMORY CACHE (Data-Oriented Design)
        // ==========================================
        // Kept alive between frames to reuse the reserved memory capacity
        std::vector<PhysicsBody> activeBodies;
        std::vector<Physics::OBB> obbCache;
        std::vector<TriggerAreaComponent*> activeTriggers;

        // ==========================================
        // PHYSICS PIPELINE STEPS
        // ==========================================

        /// Fills lists and bakes user geometric data into pure OBBs.
        void PrePass();

        /// The double loop that evaluates collisions and applies the Iterative Solver.
        void SolveCollisions();

        /// Separate evaluation for intangible trigger zones.
        void DetectTriggers();
    };
}