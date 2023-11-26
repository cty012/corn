#include <corn/ui/ui_label.h>
#include "../media/text_render_impl.h"

namespace corn {
    UILabel::UILabel(WidgetID id, std::string name, UIManager& uiManager, const RichText& text)
            : UIWidget(UIType::LABEL, id, std::move(name), uiManager), textRender_(text) {}

    const RichText& UILabel::getText() const {
        return this->textRender_.getText();
    }

    void UILabel::setText(const RichText& richText) {
        this->textRender_.setText(richText);
    }

    const TextRender& UILabel::getTextRender() const {
        return this->textRender_;
    }
}
