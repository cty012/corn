#include <corn/ui/ui_label.h>
#include "../media/rich_text_impl.h"

namespace corn {
    UILabel::UILabel(WidgetID id, std::string name, UIManager& uiManager, RichText text)
        : UIWidget(UIType::LABEL, id, std::move(name), uiManager), text(std::move(text)) {}

    const RichText& UILabel::getText() const {
        return this->text;
    }

    void UILabel::setText(const RichText& newText) {
        this->text = newText;
    }
}
