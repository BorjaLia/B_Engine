#pragma once

#include <string>

#include "Component.h"
#include "../utils/Types.h"
#include "../resources/Font.h"

namespace Engine
{
    class IRenderer;

    enum class TextAlignment
    {
        Left,
        Center,
        Right
    };

    /// Renders text using a custom loaded Font atlas.
    /// @ingroup Components
    class TextComponent : public Component
    {
    public:
        // Basic Data
        std::string text;
        Font* font;

        // Visual Style
        float fontSize;
        Color tint;
        float extraLetterSpacing;
        float extraLineSpacing;

        // Layout
        TextAlignment alignment;
        Pivot pivot;
        RenderLayer layer;

        TextComponent(Font* font = nullptr, const std::string& text = "",
            float fontSize = 32.0f, Color tint = { 255, 255, 255, 255 },
            RenderLayer layer = RenderLayer::UI);

        ~TextComponent() override = default;

        void Update(float deltaTime) override;
        void Draw(RendererBase* renderer) override;

        std::string ToString() const override;

        /// Calculates the actual bounds of the text scaled to the screen size.
        Vector2f GetBounds() const;

        void SetText(const std::string& newText) { text = newText; }
        void SetFont(Font* newFont) { font = newFont; }
    };
}