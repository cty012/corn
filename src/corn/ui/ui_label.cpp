#include <corn/ui/ui_label.h>
#include "../media/text_render_impl.h"

namespace corn {
    UILabel::UILabel(WidgetID id, std::string name, UIManager& uiManager, const RichText& text) noexcept
            : UIWidget(UIType::LABEL, id, std::move(name), uiManager), textRender_(text) {}

    Vec2 UILabel::getNaturalSize() const noexcept {
        return this->textRender_.getNaturalSize();
    }

    const RichText& UILabel::getText() const noexcept {
        return this->textRender_.getText();
    }

    void UILabel::setText(const RichText& richText) {
        this->textRender_.setText(richText);
    }

    TextRender& UILabel::getTextRender() noexcept {
        return this->textRender_;
    }

    const TextRender& UILabel::getTextRender() const noexcept {
        return this->textRender_;
    }
}
