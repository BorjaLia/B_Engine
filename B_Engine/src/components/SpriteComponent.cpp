#include "SpriteComponent.h"

#include <sstream>

#include "../scenes/Node.h"
#include "TransformComponent.h"
#include "../graphics/RendererBase.h"

namespace Engine
{
    SpriteComponent::SpriteComponent(const Texture2D& texture, Pivot pivot, Color tint, RenderLayer layer)
        : texture(texture), pivot(pivot), tint(tint), layer(layer)
    {
    }

    void SpriteComponent::Update(float /*deltaTime*/)
    {
    }

    void SpriteComponent::Draw(RendererBase* renderer)
    {
        if (owner == nullptr || owner->transform == nullptr || renderer == nullptr) return;

        Vector2f position = owner->GetGlobalPosition();
        float    rotation = owner->transform->GetRotation();
        Vector2f scale = owner->transform->GetScale();

        if (targetSize.x != 0.0f && targetSize.y != 0.0f)
        {
            scale.x *= (targetSize.x / texture.size.x);
            scale.y *= (targetSize.y / texture.size.y);
        }

        renderer->SubmitSprite(layer, texture, position, rotation, scale, pivot, tint, flipX, flipY);
    }

    std::string SpriteComponent::ToString() const
    {
        std::stringstream ss;
        ss << "SpriteComponent [Color: " << tint << "]";
        return ss.str();
    }
}