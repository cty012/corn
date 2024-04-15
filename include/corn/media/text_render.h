#pragma once

namespace corn {
    struct Vec2;
    struct RichText;

    /**
     * @class TextRender
     * @brief Wrapper around RichText which auto generates rendering contexts for more efficient text rendering.
     *
     * User should not need to access this class. Other classes' API should use RichText instead.
     *
     * The constructor has a optional parameter to specify the width limit. When there is no limit, the text will be
     * rendered in a single line. If there is a limit, the text will auto-wrap at white spaces.
     *
     * @see RichText
     */
    class TextRender {
    public:
        friend class Interface;

        /**
         * @brief Constructor.
         * @param richText Rich text to wrap around.
         */
        explicit TextRender(const RichText& richText);

        /**
         * @brief Constructor.
         * @param richText Rich text to wrap around.
         * @param width Width of the text area. If text exceeds this width, it should auto wrap at white spaces.
         */
        TextRender(const RichText& richText, float width);

        /// @brief Getter for the rich text inside.
        [[nodiscard]] const RichText& getText() const noexcept;

        /// @brief Setter for the rich text inside.
        void setText(const RichText& richText);

        /// @return Size of the text area.
        [[nodiscard]] const Vec2& getSize() const noexcept;

        /**
         * @return Natural size of the text area.
         *
         * The natural size of the text is the size of the text if there is no width limit.
         */
        [[nodiscard]] const Vec2& getNaturalSize() const noexcept;

        /**
         * @brief Set the new width limit of the RichText.
         * @param width New width limit.
         * @throw std::invalid_argument If invalid input (width is negative).
         */
        void setWidth(float width);

        /// @brief Remove any existing width limit.
        void setWidthNoLimit();

        /// @brief Getter for the line padding.
        [[nodiscard]] float getLinePadding() const noexcept;

        /// @brief Setter for the line padding.
        void setLinePadding(float linePadding);

    private:
        /// @brief Line padding. Space added to both top and bottom of line.
        float linePadding_;

        /// @brief Pimpl idiom.
        class TextRenderImpl;
        TextRenderImpl* impl_;
    };
}
