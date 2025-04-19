#pragma once

#include <corn/util/rich_text.h>

namespace corn {
    class RichTextRenderer;

    enum class WrapStyle {
        WORD, CHARACTER, NONE,
    };

    enum class TextAlign {
        LEFT, RIGHT, CENTER, JUSTIFIED,
    };

    /**
     * @class RichTextFrame
     * @brief Frame for rendering rich text.
     */
    class RichTextFrame {
    public:
        /// @brief Constructor.
        explicit RichTextFrame(const RichText& richText = RichText()) noexcept;

        /// @brief Destructor.
        ~RichTextFrame() noexcept;

        RichTextFrame(const RichTextFrame& other) = delete;
        RichTextFrame& operator=(const RichTextFrame& other) = delete;

        // Getters and Setters
        [[nodiscard]] const RichText& getRichText() const noexcept;
        void setRichText(const RichText& richText) noexcept;
        [[nodiscard]] float getMaxWidth() const noexcept;
        void setMaxWidth(float maxWidth) noexcept;
        [[nodiscard]] WrapStyle getWrapStyle() const noexcept;
        void setWrapStyle(WrapStyle wrapStyle) noexcept;
        [[nodiscard]] TextAlign getTextAlign() const noexcept;
        void setTextAlign(TextAlign textAlign) noexcept;
        [[nodiscard]] const Vec2f& getNaturalSize() const;
        [[nodiscard]] const Vec2f& getSize() const;
        [[nodiscard]] RichTextRenderer* getRichTextRenderer() const;

    private:
        /// @brief The rich text in the frame.
        RichText richText_;

        /// @brief The maximum width of the text.
        float maxWidth_;

        /// @brief The word wrapping style (default: WrapStyle::WORD).
        WrapStyle wrapStyle_;

        /// @brief The text alignment (default: TextAlign::LEFT).
        TextAlign textAlign_;

        /// @brief The natural size of the text. Depends on the rich text.
        mutable Vec2f naturalSize_;
        mutable bool naturalSizeDirty_;

        /// @brief The size of the text. Depends on the rich text and max width.
        mutable Vec2f size_;
        mutable bool sizeDirty_;

        /// @brief The rich text renderer.
        RichTextRenderer* richTextRenderer_;
        mutable bool richTextRendererRichTextDirty_;
        mutable bool richTextRendererFormatDirty_;
    };
}
