#pragma once

#include <string>

#include "../math/Vector2.h"
#include "../math/Vector3.h"
#include "../math/Matrix4x4.h"

namespace Engine
{
    // Forward declaration to avoid circular dependencies
    class Node;

    /// Defines the position, rotation, and scale of a Node in 3D space.
    /// Pure mathematical data structure (POD-like). Does NOT inherit from Component.
    /// @ingroup Scenes
    class Transform
    {
    public:
        Transform();
        ~Transform() = default;

        std::string ToString() const;

#pragma region API
        // Setters (Overloaded for 3D and 2D convenience)
        void SetPosition(const Vector3f& pos);
        void SetPosition(const Vector2f& pos); // Keeps Z intact

        void SetEulerAngles(const Vector3f& rotRadians);
        void SetRotation2D(float rotDegrees); // Modifies only Z (Roll)

        void SetScale(const Vector3f& scale);
        void SetScale(const Vector2f& scale); // Keeps Z intact

        // Unified Getters 
        Vector3f GetPosition() const { return localPosition; }
        Vector3f GetScale()  const { return localScale; }
        
        Vector3f GetEulerAngles() const { return localRotation; }
        float    GetRotation2D() const;
        
        Vector3f GetGlobalPosition() const;
        Vector3f GetGlobalEulerAngles() const;

        /// Returns the normalized forward vector (Z-axis) in global space.
        Vector3f GetForward() const;
        /// Returns the normalized right vector (X-axis) in global space.
        Vector3f GetRight() const;
        /// Returns the normalized up vector (Y-axis) in global space.
        Vector3f GetUp() const;
#pragma endregion

        /// Recalculates the internal matrices if the transform has been modified.
        void UpdateTransform() const;

        /// Retrieves the final calculated hierarchy matrix.
        const Matrix4x4& GetGlobalMatrix() const;

        /// Links the transform to its node (Used temporarily for dirty flag propagation)
        void SetOwner(Node* node) { owner = node; }

    private:
        Node* owner = nullptr;

        Vector3f localPosition;
        Vector3f localRotation; // Euler angles in Radians (Pitch, Yaw, Roll)
        Vector3f localScale;

        mutable Matrix4x4 localMatrix;
        mutable Matrix4x4 globalMatrix;

        mutable bool isDirty; // Optimization flag

        void SetChildrenDirty();
    };
}