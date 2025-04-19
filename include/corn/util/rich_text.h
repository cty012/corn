#pragma once

#include <string>
#include <vector>
#include <corn/media/font_manager.h>
#include <corn/util/color.h>

namespace corn {
    /**
     * @class TextStyle
     * @brief Uses a builder-like pattern to specify text styles.
     *
     * @see RichText
     */
    struct TextStyle {
        /// @brief Pointer to the font family to use.
        const FontFamily* fontFamily;

        /// @brief Size of the text segment.
        float size;

        /// @brief Color of the text segment.
        Color color;

        /// @brief Variant of the font.
        FontVariant fontVariant;

        /// @brief Whether the text has underline.
        bool underline;

        /// @brief Position of the text segment.
        FontPosition position;

        /// @brief Simple constructor.
        TextStyle(const FontFamily* fontFamily, float size, Color color) noexcept;

        /// @brief Complete constructor.
        TextStyle(
                const FontFamily* fontFamily, float size, Color color, const FontVariant& fontVariant,
                bool underline, FontPosition position) noexcept;

        friend bool operator==(const TextStyle& lhs, const TextStyle& rhs) noexcept = default;
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

            friend bool operator==(const Segment& lhs, const Segment& rhs) noexcept = default;
        };

        /// @brief Each segment represents a piece of text with a uniform style.
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
        RichText& addText(std::string text, TextStyle style) noexcept;

        /// @return The text without any styles.
        [[nodiscard]] std::string getString() const noexcept;

        friend bool operator==(const RichText& lhs, const RichText& rhs) noexcept = default;
    };
}
