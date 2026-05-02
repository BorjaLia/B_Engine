#pragma once

#include "Physics.h"

namespace Engine
{
    /// Interface for components that need to respond to physics collisions.
    /// @ingroup Physics
    class ICollisionListener
    {
    public:
        virtual ~ICollisionListener() = default;

        /// Called by the PhysicsSystem when a collision involving this object is resolved.
        /// @param hit The collision manifold containing impact normals, depth, and contact points.
        virtual void OnCollision(const Physics::CollisionManifold& hit) = 0;
    };
}