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
        float size;

        /// @brief Color of the text segment.
        Color color;

        /// @brief Variant of the font to use.
        FontWeight weight;
        FontSlant slant;
        FontDecoration decoration;

        /// @brief Simple constructor.
        TextStyle(const Font* font, float size, Color color) noexcept;

        /// @brief Complete constructor.
        TextStyle(
                const Font* font, float size, Color color,
                FontWeight weight, FontSlant slant, FontDecoration decoration) noexcept;

        /// @return A NEW TextStyle object with the updated font.
        [[nodiscard]] TextStyle setFont(const Font* newFont) const noexcept;

        /// @return A NEW TextStyle object with the updated size.
        [[nodiscard]] TextStyle setSize(float newSize) const noexcept;

        /// @return A NEW TextStyle object with the updated color.
        [[nodiscard]] TextStyle setColor(Color newColor) const noexcept;

        /// @return A NEW TextStyle object with the updated weight.
        [[nodiscard]] TextStyle setWeight(FontWeight newWeight) const noexcept;

        /// @return A NEW TextStyle object with the updated slant.
        [[nodiscard]] TextStyle setSlant(FontSlant newSlant) const noexcept;

        /// @return A NEW TextStyle object with the updated decoration.
        [[nodiscard]] TextStyle setDecoration(FontDecoration newDecoration) const noexcept;
    };

    enum class TextAlign {
        NATURAL, LEFT, RIGHT, CENTER, JUSTIFIED,
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
            std::string text;

            /// @brief The style of the segment.
            TextStyle style;

            /// @brief Constructor.
            Segment(std::string text, TextStyle style) noexcept;
        };

        /// @brief Each segment represent a piece of text with uniform style.
        std::vector<Segment> segments;

        /// @brief Text alignment.
        TextAlign textAlign = TextAlign::NATURAL;

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
        RichText& addText(std::string text, TextStyle style) noexcept;

        /// @return The text without any styles.
        [[nodiscard]] std::string getString() const noexcept;
    };
}
