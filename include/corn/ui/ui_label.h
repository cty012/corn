#pragma once

#include <corn/media/rich_text_frame.h>
#include <corn/util/rich_text.h>
#include <corn/ui/ui_widget.h>

namespace corn {
    class RichTextRenderer;

    /**
     * @class UILabel
     * @brief Widget that displays text.
     *
     * @see UIWidget
     */
    class UILabel : public UIWidget {
    public:
        // UIManager need access to ctor/dtor
        friend class UIManager;

        [[nodiscard]] Vec2f getNaturalSize() const noexcept override;

        /// @brief Getter for the rich text.
        [[nodiscard]] const RichText& getRichText() const noexcept;

        /// @brief Setter for the rich text.
        void setRichText(const RichText& richText);

        /// @brief Getter for the rich text frame.
        [[nodiscard]] RichTextFrame& getRichTextFrame() noexcept;

        /// @brief Getter for the rich text frame.
        [[nodiscard]] const RichTextFrame& getRichTextFrame() const noexcept;

    private:
        /// @brief Constructor.
        UILabel(WidgetID id, std::string name, UIManager& uiManager, const RichText& text) noexcept;

        /// @brief The rich text renderer.
        RichTextFrame richTextFrame_;
    };
}
