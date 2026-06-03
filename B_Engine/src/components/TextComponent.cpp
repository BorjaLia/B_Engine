#include "TextComponent.h"

#include <sstream>
#include <vector>

#include "../scenes/Node.h"
#include "TransformComponent.h"
#include "../graphics/RendererBase.h"

namespace Engine
{
    TextComponent::TextComponent(Font* font, const std::string& text, float fontSize, Color tint, RenderLayer layer)
        : font(font), text(text), fontSize(fontSize), tint(tint), layer(layer),
        extraLetterSpacing(0.0f), extraLineSpacing(0.0f),
        alignment(TextAlignment::Left), pivot(Pivot::TopLeft)
    {
    }

    void TextComponent::Update(float /*deltaTime*/)
    {
    }

    Vector2f TextComponent::GetBounds() const
    {
        if (!font || text.empty()) return Vector2f(0.0f, 0.0f);

        Vector2f baseBounds = font->MeasureText(text, extraLetterSpacing);
        float scale = fontSize / static_cast<float>(font->GetBaseSize());

        int lineCount = 1;
        for (char c : text) if (c == '\n') lineCount++;

        return Vector2f(
            baseBounds.x * scale,
            (baseBounds.y * scale) + (extraLineSpacing * (lineCount - 1) * scale)
        );
    }

    void TextComponent::Draw(RendererBase* renderer)
    {
        if (!font || text.empty() || owner == nullptr || renderer == nullptr) return;

        Vector3f globalPos = owner->transform.GetGlobalPosition();
        float rotation = owner->transform.GetRotation2D();

        Vector3f transformScale = owner->transform.GetScale();

        float fontScale = fontSize / static_cast<float>(font->GetBaseSize());
        Vector2f finalScale = { fontScale * transformScale.x, fontScale * transformScale.y };

        Vector2f bounds = GetBounds();

        Vector3f startPos = globalPos;
        Vector2f pivotMult = GetPivotMultiplier(pivot);

        startPos.x -= bounds.x * pivotMult.x;
        startPos.y -= bounds.y * pivotMult.y;

        std::vector<std::string> lines;
        std::stringstream ss(text);
        std::string line;

        while (std::getline(ss, line, '\n'))
        {
            lines.push_back(line);
        }

        // FIX: Raylib stores glyph offsetY in Y-down screen space (positive = shift down).
        // For UI layer: no Y-flip in renderer, apply offset as-is -> correct.
        // For World layer: renderer Y-flips the position, so a positive offsetY would shift
        // the glyph UP instead of down. Negate it so it lands correctly after the flip.
        const float yOffsetSign = (layer == RenderLayer::World) ? -1.0f : 1.0f;

        float currentY = startPos.y;

        for (const std::string& currentLine : lines)
        {
            Vector2f lineMeasure = font->MeasureText(currentLine, extraLetterSpacing);
            float scaledLineWidth = lineMeasure.x * finalScale.x;
            float currentX = startPos.x;

            if (alignment == TextAlignment::Center) currentX += (bounds.x * 0.5f) - (scaledLineWidth * 0.5f);
            else if (alignment == TextAlignment::Right)  currentX += bounds.x - scaledLineWidth;

            for (char c : currentLine)
            {
                const Glyph* g = font->GetGlyph(c);
                if (!g) continue;

                Vector3f charPos = { currentX, currentY, startPos.z };

                charPos.x += g->offset.x * finalScale.x;
                charPos.y += g->offset.y * finalScale.y * yOffsetSign;

                renderer->SubmitSprite(
                    layer, font->atlas, g->sourceRect,
                    charPos, rotation, finalScale,
                    Pivot::TopLeft, tint
                );

                currentX += (g->advanceX + extraLetterSpacing) * finalScale.x;
            }

            currentY += (font->GetLineHeight() + extraLineSpacing) * finalScale.y;
        }
    }

    std::string TextComponent::ToString() const
    {
        std::stringstream ss;
        ss << "TextComponent [Text: '" << text.substr(0, 10) << (text.length() > 10 ? "..." : "")
            << "', Size: " << fontSize << ", Layer: " << static_cast<int>(layer) << "]";
        return ss.str();
    }
}