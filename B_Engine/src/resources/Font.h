#pragma once

#include <unordered_map>
#include <string>

#include "../utils/Types.h"

namespace Engine
{
    /// Mathematical information for a single character glyph.
    struct Glyph
    {
        Rect sourceRect; // Where the letter is inside the atlas (in pixels)
        Vector2f offset; // How much to offset it when drawing
        float advanceX = 1;  // Space until the next letter starts
    };

    /// Represents a loaded font, managing its glyph data and texture atlas.
    class Font
    {
    public:
        Font() = default;
        ~Font() = default;

        // Setters used by the Renderer when building the font
        void AddGlyph(char c, const Glyph& g) { glyphs[c] = g; }
        void SetBaseSize(int size) { baseSize = size; }
        void SetLineHeight(float height) { lineHeight = height; }

        // Getters
        int GetBaseSize() const { return baseSize; }
        float GetLineHeight() const { return lineHeight; }

        /// Retrieves the mathematical data for a specific character.
        /// @return Pointer to the glyph, or nullptr if not found.
        const Glyph* GetGlyph(char c) const
        {
            auto it = glyphs.find(c);
            if (it != glyphs.end()) return &it->second;
            return nullptr;
        }

        /// Calculates the total width and height of a text string without drawing it.
        /// @param text The string to measure.
        /// @param spacing The extra horizontal space added between characters.
        /// @return A Vector2f containing the total width and height in pixels.
        Vector2f MeasureText(const std::string& text, float spacing) const;

        // The main image containing the entire alphabet
        Texture2D atlas;

    private:
        std::unordered_map<char, Glyph> glyphs; // The character dictionary
        int baseSize = 0;                       // Size used during generation
        float lineHeight = 0.0f;                // Vertical space between lines (\n)
    };
}