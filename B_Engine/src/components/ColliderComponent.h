#pragma once

#include <vector>

#include "Component.h"
#include "../utils/Types.h" 
#include "../utils/Math.h"  

namespace Engine
{
    class ICollisionListener;

    /// Defines a physical boundary for an entity.
    /// Used by the PhysicsSystem to detect overlaps and resolve forces.
    /// @ingroup Components
    class ColliderComponent : public Component
    {
    public:
        /// Constructor with default values (creates a red centered 50x50 box).
        ColliderComponent(
            const Shape& initialShape = RectangleShape{ {50.0f, 50.0f} },
            const Vector2f& offset = { 0.0f, 0.0f },
            bool autoFit = false,
            const Color& color = { 255, 0, 0, 255 }
        );

        ~ColliderComponent() override;

        void Start() override;

        /// Overridden to send the collision shape to the debug render queue.
        void DebugDraw(class RendererBase* renderer) override;

        Shape GetShape() const { return shape; }
        void SetShape(const Shape& newShape) { shape = newShape; }

        Vector2f GetOffset() const { return offset; }
        void SetOffset(const Vector2f& newOffset) { offset = newOffset; }

        void SetDebugColor(const Color& color) { debugColor = color; }

        void MarkObbDirty();

    private:
        Shape shape;
        Vector2f offset;
        Color debugColor;
        bool autoFitToSprite;
    };
}