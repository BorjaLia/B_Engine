#pragma once

#include "../utils/Types.h"

namespace Engine::Physics
{
#pragma region Physics Constants
    constexpr int MAX_CONTACT_POINTS = 2;
#pragma endregion

#pragma region Data Structures
    /// AAA structure to resolve dynamic physics collisions.
    /// @ingroup Physics
    struct CollisionManifold
    {
        bool isColliding = false;

        Vector2f normal = { 0.0f, 0.0f }; // Impact direction (points from B to A)
        float depth = 0.0f;               // Penetration depth (Scalar)

        // Exact world-space impact points (1 for Circles, 1 or 2 for Boxes)
        Vector2f contactPoints[MAX_CONTACT_POINTS];
        int contactCount = 0;
    };

    /// Axis-Aligned Bounding Box (Ultra fast, does not rotate)
    struct AABB
    {
        Vector2f min;
        Vector2f max;
    };

    /// Oriented Bounding Box (Precise, rotates with the Transform)
    struct OBB
    {
        Vector2f vertices[4];
        Vector2f axisX; // Local "Forward/Right" vector
        Vector2f axisY; // Local "Up" vector
    };
#pragma endregion

#pragma region Geometry Factories
    AABB GetAABB(const Vector2f& center, const Vector2f& size);
    OBB GetOBB(const Vector2f& center, const Vector2f& size, float rotationRadians);
#pragma endregion

#pragma region Detection Systems (Radar)
    // Specific optimized overloads (Used internally by the engine)
    CollisionManifold CheckCollision(const Vector2f& posA, float radiusA, const Vector2f& posB, float radiusB);
    CollisionManifold CheckCollision(const Vector2f& circlePos, float radius, const OBB& obb);
    CollisionManifold CheckCollision(const OBB& a, const OBB& b);

    /// Universal Dispatcher (Used for Triggers, UI, or user queries).
    /// Dynamically constructs OBBs requiring rotation parameters.
    CollisionManifold CheckCollision(const Shape& shapeA, const Vector2f& posA, float rotA,
        const Shape& shapeB, const Vector2f& posB, float rotB);
#pragma endregion
}