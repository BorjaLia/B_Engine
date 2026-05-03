#include "ColliderComponent.h"

#include <cmath>
#include <algorithm>

#include "../scenes/Node.h"
#include "../graphics/RendererBase.h"
#include "SpriteComponent.h"
#include "AnimatedSpriteComponent.h"

namespace Engine
{
    ColliderComponent::ColliderComponent(const Shape& initialShape, const Vector2f& offset, bool autoFit, const Color& color)
        : shape(initialShape), offset(offset), autoFitToSprite(autoFit), debugColor(color)
    {
    }

    void ColliderComponent::Start()
    {
        if (!autoFitToSprite || owner == nullptr) return;

        auto* sprite = owner->GetComponent<SpriteComponent>();
        auto* animator = owner->GetComponent<AnimatedSpriteComponent>();

        // If it doesn't have either, we abort the auto-fit
        if (!animator && !sprite) return;

        Pivot pivot;
        Vector2f baseSize; // Stores the base size (Frame or Texture)

        if (animator)
        {
            pivot = animator->pivot;
            Vector2f target = animator->GetTargetSize();

            if (target.x != 0.0f && target.y != 0.0f)
                baseSize = target; // Use the forced target size
            else
                baseSize = animator->GetCurrentFrameSize(); // Use the original frame size
        }
        else
        {
            pivot = sprite->pivot;
            Vector2f target = sprite->GetTargetSize();

            if (target.x != 0.0f && target.y != 0.0f)
                baseSize = target;
            else
                baseSize = { static_cast<float>(sprite->texture.size.x), static_cast<float>(sprite->texture.size.y) };
        }

        // 1. Get the final transform scale
        Vector2f scale = owner->transform->GetScale();

        // 2. Calculate the real scaled size based on 'baseSize'
        float finalWidth = baseSize.x * std::abs(scale.x);
        float finalHeight = baseSize.y * std::abs(scale.y);

        // 3. Bake the final size into the Shape
        if (std::holds_alternative<RectangleShape>(shape))
        {
            shape = RectangleShape{ {finalWidth, finalHeight} };
        }
        else if (std::holds_alternative<CircleShape>(shape))
        {
            shape = CircleShape{ std::max(finalWidth, finalHeight) / 2.0f };
        }

        // 4. THE PIVOT MAGIC!
        Vector2f pivotMult = GetPivotMultiplier(pivot);

        offset.x = (0.5f - pivotMult.x) * finalWidth;
        offset.y = (0.5f - pivotMult.y) * finalHeight;
    }

    void ColliderComponent::DebugDraw(RendererBase* renderer)
    {
        if (owner == nullptr || renderer == nullptr) return;

        Vector2f globalPos = owner->GetGlobalPosition();
        Vector2f colliderPos = { globalPos.x + offset.x, globalPos.y + offset.y };

        float rot = owner->transform->GetRotation();
        float rotRad = rot * (3.14159f / 180.0f);

        // Draw the main collider boundaries
        renderer->SubmitDebugShape(RenderLayer::World, GetShape(), colliderPos, rot, debugColor);

        // Draw an orientation pointer (cyan line)
        float pointerLength = 40.0f;
        float pointerThickness = 2.0f;
        RectangleShape pointerShape{ {pointerThickness, pointerLength} };

        Vector2f localPointerOffset = { 0.0f, pointerLength / 2.0f };
        Vector2f rotatedPointerOffset = Matrix3x3::Rotation(rotRad) * localPointerOffset;
        Vector2f pointerPos = colliderPos;

        Color pointerColor = { 0, 255, 255, 255 }; // Cyan

        renderer->SubmitDebugShape(RenderLayer::World, pointerShape, pointerPos, rot, pointerColor);
    }
}