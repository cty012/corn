/**
 * @file rich_text.h
 * @brief Defines the RichText class and a container for storing text styles.
 */

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
            std::u8string str;
            TextStyle style;
        };

        /// @brief Each segment represent a piece of text with uniform style.
        std::vector<Segment> segments;

        /// @brief Constructor.
        RichText();
        /// @brief Destructor.
        ~RichText();

        /**
         * @brief Appends a segment with the specified text string and text style.
         * @param text The text string.
         * @param style The text style.
         * @return A reference to itself.
         */
        RichText& addText(const std::u8string& text, TextStyle style);
    };
}
