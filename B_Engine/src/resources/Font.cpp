#include "Font.h"

#include <algorithm> 

namespace Engine
{
    Vector2f Font::MeasureText(const std::string& text, float spacing) const
    {
        if (text.empty() || glyphs.empty())
        {
            return Vector2f(0.0f, 0.0f);
        }

        float maxWidth = 0.0f;
        float currentWidth = 0.0f;
        float currentHeight = lineHeight; // Assume there is at least 1 line

        for (size_t i = 0; i < text.length(); ++i)
        {
            char c = text[i];

            // If there's a line break, evaluate if this line was the widest one
            if (c == '\n')
            {
                maxWidth = std::max(maxWidth, currentWidth);
                currentWidth = 0.0f;           // Reset X cursor
                currentHeight += lineHeight;   // Move down one line in Y
                continue;
            }

            const Glyph* g = GetGlyph(c);
            if (g != nullptr)
            {
                currentWidth += g->advanceX;

                // Only add spacing if it's NOT the last letter of the current line
                if (i + 1 < text.length() && text[i + 1] != '\n')
                {
                    currentWidth += spacing;
                }
            }
        }

        // After the loop, check the final line width one last time
        maxWidth = std::max(maxWidth, currentWidth);

        return Vector2f(maxWidth, currentHeight);
    }
}