#pragma once

#include <string>

#include "Component.h"
#include "../utils/Types.h"
#include "../utils/Math.h"

namespace Engine
{
    class IRenderer;

    /// Renders a static 2D texture at the node's transform position.
    /// @ingroup Components
    class SpriteComponent : public Component
    {
    public:
        Texture2D texture;
        Color tint;
        Pivot pivot;
        RenderLayer layer;

        SpriteComponent(const Texture2D& texture, Pivot pivot = Pivot::TopLeft, Color tint = { 255, 255, 255, 255 }, RenderLayer layer = RenderLayer::World);
        ~SpriteComponent() override = default;

        void Update(float deltaTime) override;
        void Draw(RendererBase* renderer) override;

        std::string ToString() const override;

        void SetTexture(const Texture2D& newTexture) { texture = newTexture; }
        void SetTint(const Color& newTint) { tint = newTint; }

        void SetFlipX(bool flip) { flipX = flip; }
        void SetFlipY(bool flip) { flipY = flip; }
        bool GetFlipX() const { return flipX; }
        bool GetFlipY() const { return flipY; }

        void SetTargetSize(const Vector2f& size) { targetSize = size; }
        Vector2f GetTargetSize() const { return targetSize; }

        void SetLayer(RenderLayer newLayer) { layer = newLayer; }
        RenderLayer GetLayer() const { return layer; }

    private:
        bool flipX = false;
        bool flipY = false;

        Vector2f targetSize = { 0.0f, 0.0f };
    };
}