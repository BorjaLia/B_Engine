#pragma once

#include <string>

#include "Component.h"
#include "../utils/Math.h"

namespace Engine
{
    class IRenderer;

    /// Defines the position, rotation, and scale of a Node in 2D space.
    /// Calculates global matrices hierarchically based on parent transforms.
    /// @ingroup Components
    class TransformComponent : public Component
    {
    public:
        TransformComponent();
        ~TransformComponent() override = default;

        void Start() override;
        void DebugDraw(RendererBase* renderer) override;

        std::string ToString() const override;

        // Setters automatically flag the transform hierarchy as "dirty"
        void SetPosition(const Vector2f& pos);
        void SetRotation(float rot);
        void SetScale(const Vector2f& scale);

        Vector2f GetPosition() const { return localPosition; }
        float    GetRotation() const { return localRotation; }
        Vector2f GetScale()    const { return localScale; }

        /// Recalculates the internal matrices if the transform has been modified.
        void UpdateTransform();

        /// Retrieves the final calculated hierarchy matrix.
        const Matrix3x3& GetGlobalMatrix();

        /// Utility function to extract the global position directly from the matrix.
        Vector2f GetGlobalPosition();

    private:
        Vector2f localPosition;
        float localRotation; // In degrees
        Vector2f localScale;

        Matrix3x3 localMatrix;
        Matrix3x3 globalMatrix;

        bool isDirty; // Optimization flag

        void SetChildrenDirty();
    };
}