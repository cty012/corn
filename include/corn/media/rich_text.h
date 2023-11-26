#pragma once

#include <string>
#include <vector>
#include <corn/media/font.h>
#include <corn/util/color.h>

namespace corn {
    struct TextStyle {
        const Font* font;
        size_t size;
        Color color;
        FontVariant variant;

        /// @brief Simple constructor.
        TextStyle(const Font* font, size_t size);
        /// @brief Complete constructor.
        TextStyle(const Font* font, size_t size, const Color& color, FontVariant variant);
        TextStyle setFont(const Font* newFont);
        TextStyle setSize(size_t newSize);
        TextStyle setColor(const Color& newColor);
        TextStyle setVariant(FontVariant newVariant);
    };

    class RichText {
    public:
        /// @brief Stores a pair of text string and text style.
        struct Segment;

        /// @brief Each segment represent a piece of text with uniform style.
        std::vector<Segment*> segments;

        /// @brief Constructor.
        RichText();
        /// @brief Destructor.
        ~RichText();
        RichText(const RichText& other);
        RichText& operator=(const RichText& other);
        RichText(RichText&& other) noexcept;
        RichText& operator=(RichText&& other) noexcept;

        /**
         * @brief Appends a segment with the specified text string and text style.
         * @param text The text string.
         * @param style The text style.
         * @return A reference to itself.
         */
        RichText& addText(const std::u8string& text, TextStyle style);
    };

    const TextStyle& getStyle(RichText::Segment* segment);

    /// @brief Change the text displayed by the segment.
    void setString(RichText::Segment* segment, const std::u8string& str);
    void setFont(RichText::Segment* segment, const Font* font);
    void setSize(RichText::Segment* segment, unsigned int size);
    void setColor(RichText::Segment* segment, Color color);
    void setVariant(RichText::Segment* segment, FontVariant variant);
}
