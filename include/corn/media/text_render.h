#pragma once

#include <corn/geometry/vec2.h>
#include <corn/util/rich_text.h>

namespace corn {
    class TextRender {
    public:
        friend class Interface;

        /// @brief Constructor.
        explicit TextRender(const RichText& richText);
        TextRender(const RichText& richText, float width);

        /// @brief Getter for the rich text.
        [[nodiscard]] const RichText& getText() const;
        /// @brief Setter for the rich text.
        void setText(const RichText& richText);

        /// @return Size of the text area.
        [[nodiscard]] const Vec2& getSize() const;
        /**
         * @brief Set the new width limit of the RichText.
         * @param width New width limit.
         * @throw std::invalid_argument If invalid input (width is negative).
         */
        void setWidth(float width);
        /// @brief Remove any existing width limit.
        void setWidthNoLimit();

    private:
        class TextRenderImpl;
        TextRenderImpl* impl_;
    };
}
