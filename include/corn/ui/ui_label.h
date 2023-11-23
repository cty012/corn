#pragma once

#include <corn/media/rich_text.h>
#include <corn/ui/ui_widget.h>

namespace corn {
    class UILabel : public UIWidget {
    public:
        // UIManager need access to ctor/dtor
        friend class UIManager;

        [[nodiscard]] const RichText& getText() const;
        void setText(const RichText& newText);

        [[nodiscard]] Vec2 getNaturalSize() const;

    private:
        UILabel(WidgetID id, std::string name, UIManager& uiManager, RichText text);

        /// @brief The text to be displayed.
        RichText text;
    };
}
