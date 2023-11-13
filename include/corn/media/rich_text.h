#pragma once

#include <vector>
#include <corn/media/font.h>
#include <corn/util/color.h>

namespace corn {
    struct TextStyle {
        const Font* font;
        size_t size;
        Color color;
        FontVariant variant;
        TextStyle(const Font* font, size_t size);
        TextStyle& setFont(const Font* newFont);
        TextStyle& setSize(size_t newSize);
        TextStyle& setColor(const Color& newColor);
        TextStyle& setVariant(FontVariant newVariant);
    };

    class RichText {
    public:
        RichText();
        ~RichText();
        RichText(const RichText& other);
        RichText& operator=(const RichText& other);
        RichText(RichText&& other) noexcept;
        RichText& operator=(RichText&& other) noexcept;

        /**
         * Appends a segment with the specified text string and text style.
         * @param text The text string.
         * @param style The text style.
         * @return A reference to itself.
         */
        RichText& addText(const std::wstring& text, TextStyle style);

    private:
        /// @brief Stores a pair of text string and text style.
        class Segment;

        /// @brief Each segment represent a piece of text with uniform style.
        std::vector<Segment*> segments;
    };
}
