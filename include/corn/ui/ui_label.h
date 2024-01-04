#pragma once

#include <corn/media/text_render.h>
#include <corn/ui/ui_widget.h>

namespace corn {
    struct RichText;

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

        /// @brief Getter for the rich text.
        [[nodiscard]] const RichText& getText() const noexcept;

        /// @brief Setter for the rich text.
        void setText(const RichText& richText);

        /// @brief Getter for the text render.
        [[nodiscard]] TextRender& getTextRender() noexcept;
        [[nodiscard]] const TextRender& getTextRender() const noexcept;

    private:
        /// @brief Constructor.
        UILabel(WidgetID id, std::string name, UIManager& uiManager, const RichText& text) noexcept;

        /// @brief The text to be rendered.
        TextRender textRender_;
    };
}
