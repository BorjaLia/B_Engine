#include "TriggerAreaComponent.h"

#include <algorithm>
#include <cmath>

#include "../scenes/Node.h"
#include "../graphics/RendererBase.h"
#include "SpriteComponent.h"
#include "AnimatedSpriteComponent.h"

namespace Engine
{
    TriggerAreaComponent::TriggerAreaComponent(const Shape& initialShape, const Vector2f& offset, bool autoFit, const Color& color)
        : shape(initialShape), offset(offset), autoFitToSprite(autoFit), debugColor(color)
    {
    }

    void TriggerAreaComponent::Start()
    {
        // Early Exit! If autofit is disabled or there is no owner, do nothing.
        if (!autoFitToSprite || owner == nullptr) return;

        if (autoFitToSprite && owner)
        {
            auto* sprite = owner->GetComponent<SpriteComponent>();
            auto* animator = owner->GetComponent<AnimatedSpriteComponent>();

            // Abort if neither component is found
            if (!animator && !sprite) return;

            Pivot pivot;
            Vector2f baseSize; // Store the base size (Frame or Texture)

            if (animator)
            {
                pivot = animator->pivot;
                baseSize = animator->GetCurrentFrameSize();
            }
            else
            {
                pivot = sprite->pivot;
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
    }

    void TriggerAreaComponent::DebugDraw(RendererBase* renderer)
    {
        if (owner == nullptr || renderer == nullptr) return;

        Vector2f globalPos = owner->GetGlobalPosition();
        Vector2f triggerPos = { globalPos.x + offset.x, globalPos.y + offset.y };

        float rot = owner->transform->GetRotation();

        renderer->SubmitDebugShape(RenderLayer::World, shape, triggerPos, rot, debugColor);
    }
}