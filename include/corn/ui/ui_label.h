#pragma once

#include <corn/media/text_render.h>
#include <corn/ui/ui_widget.h>
#include <corn/util/rich_text.h>

namespace corn {
    class UILabel : public UIWidget {
    public:
        // UIManager need access to ctor/dtor
        friend class UIManager;

        [[nodiscard]] const RichText& getText() const;
        void setText(const RichText& richText);

        [[nodiscard]] const TextRender& getTextRender() const;

    private:
        /// @brief Constructor.
        UILabel(WidgetID id, std::string name, UIManager& uiManager, const RichText& text);

        /// @brief The text to be rendered.
        TextRender textRender_;
    };
}
