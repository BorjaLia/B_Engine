#pragma once

#include <functional> 

#include "Component.h"
#include "../utils/Types.h"
#include "../utils/Math.h"

namespace Engine
{
    class Node;
    class IRenderer;

    /// Defines an intangible physical boundary that emits events when other colliders enter it.
    class TriggerAreaComponent : public Component
    {
    public:
        /// Callback signature indicating which Node entered the area.
        using TriggerSignal = std::function<void(Node* otherNode)>;

        /// Default constructor (Semi-transparent yellow for debugging)
        TriggerAreaComponent(
            const Shape& initialShape = RectangleShape{ {50.0f, 50.0f} },
            const Vector2f& offset = { 0.0f, 0.0f },
            bool autoFit = false,
            const Color& color = { 255, 255, 0, 255 }
        );

        ~TriggerAreaComponent() override = default;

        void Start() override;
        void DebugDraw(IRenderer* renderer) override;

        Shape GetShape() const { return shape; }
        Vector2f GetOffset() const { return offset; }
        void SetDebugColor(const Color& color) { debugColor = color; }

        /// Binds a callback function to the trigger enter event.
        void ConnectTriggerEnter(TriggerSignal callback) { onTriggerEnter = callback; }

        /// Called by the PhysicsSystem when an overlap occurs.
        void EmitTriggerEnter(Node* otherNode)
        {
            if (onTriggerEnter) onTriggerEnter(otherNode);
        }

    private:
        Shape shape;
        Vector2f offset;
        Color debugColor;
        bool autoFitToSprite;

        TriggerSignal onTriggerEnter = nullptr;
    };
}