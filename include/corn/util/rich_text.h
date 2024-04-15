#pragma once

#include <string>
#include <vector>
#include <corn/media/font.h>
#include <corn/util/color.h>

namespace corn {
    /**
     * @class TextStyle
     * @brief Uses a builder-like pattern to specify text styles.
     *
     * @see RichText
     */
    struct TextStyle {
        /// @brief Pointer to the font to use.
        const Font* font;

        /// @brief Size of the text segment.
        size_t size;

        /// @brief Color of the text segment.
        Color color;

        /// @brief Variant of the font to use.
        FontVariant variant;

        /// @brief Simple constructor.
        TextStyle(const Font* font, size_t size) noexcept;

        /// @brief Complete constructor.
        TextStyle(const Font* font, size_t size, const Color& color, FontVariant variant) noexcept;

        /// @return A NEW TextStyle object with the updated font.
        [[nodiscard]] TextStyle setFont(const Font* newFont) const noexcept;

        /// @return A NEW TextStyle object with the updated size.
        [[nodiscard]] TextStyle setSize(size_t newSize) const noexcept;

        /// @return A NEW TextStyle object with the updated color.
        [[nodiscard]] TextStyle setColor(const Color& newColor) const noexcept;

        /// @return A NEW TextStyle object with the updated variant.
        [[nodiscard]] TextStyle setVariant(FontVariant newVariant) const noexcept;
    };

    /**
     * @class RichText
     * @brief Contains a piece of styled text encoded with utf-8.
     *
     * This class is a pure data container and does not store any cached information about text rendering.
     *
     * @see TextStyle
     * @see TextRender
     */
    struct RichText {
        /// @brief Stores a pair of text string and text style.
        struct Segment {
            /// @brief The text stored in the segment encoded in utf-8.
            std::u8string str;

            /// @brief The style of the segment.
            TextStyle style;

            /// @brief Constructor.
            Segment(std::u8string str, TextStyle style) noexcept;
        };

        /// @brief Each segment represent a piece of text with uniform style.
        std::vector<Segment> segments;

        /// @brief Constructor.
        RichText() noexcept;

        /// @brief Destructor.
        ~RichText() noexcept;

        /**
         * @brief Appends a segment with the specified text string and text style.
         * @param text The text string.
         * @param style The text style.
         * @return A reference to the rich text itself.
         */
        RichText& addText(const std::u8string& text, TextStyle style) noexcept;

        /**
         * @brief Appends a segment with the specified text string and text style.
         * @param text The text string.
         * @param style The text style.
         * @return A reference to the rich text itself.
         */
        RichText& addText(const std::string& text, TextStyle style) noexcept;
    };
}
