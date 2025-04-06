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
        float weight;
        bool italic;
        bool underline;
        FontPosition position;

        /// @brief Simple constructor.
        TextStyle(const Font* font, float size, Color color) noexcept;

        /// @brief Complete constructor.
        TextStyle(
                const Font* font, float size, Color color,
                float weight, bool italic, bool underline, FontPosition position) noexcept;

        /// @return A NEW TextStyle object with the updated font.
        [[nodiscard]] TextStyle setFont(const Font* newFont) const noexcept;

        /// @return A NEW TextStyle object with the updated size.
        [[nodiscard]] TextStyle setSize(float newSize) const noexcept;

        /// @return A NEW TextStyle object with the updated color.
        [[nodiscard]] TextStyle setColor(Color newColor) const noexcept;

        /// @return A NEW TextStyle object with the updated weight.
        [[nodiscard]] TextStyle setWeight(float newWeight) const noexcept;

        /// @return A NEW TextStyle object with the updated italic.
        [[nodiscard]] TextStyle setItalic(bool newItalic) const noexcept;

        /// @return A NEW TextStyle object with the updated underline.
        [[nodiscard]] TextStyle setUnderline(bool newUnderline) const noexcept;

        /// @return A NEW TextStyle object with the updated position.
        [[nodiscard]] TextStyle setPosition(FontPosition newPosition) const noexcept;
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

    bool operator==(const TextStyle& lhs, const TextStyle& rhs) noexcept;
    bool operator!=(const TextStyle& lhs, const TextStyle& rhs) noexcept;
    bool operator==(const RichText::Segment& lhs, const RichText::Segment& rhs) noexcept;
    bool operator!=(const RichText::Segment& lhs, const RichText::Segment& rhs) noexcept;
    bool operator==(const RichText& lhs, const RichText& rhs) noexcept;
    bool operator!=(const RichText& lhs, const RichText& rhs) noexcept;
}
