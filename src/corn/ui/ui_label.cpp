#include <corn/ui/ui_label.h>

namespace corn {
    UILabel::UILabel(WidgetID id, std::string name, UIManager& uiManager, const RichText& text) noexcept
            : UIWidget(UIType::LABEL, id, std::move(name), uiManager), richTextFrame_(text) {}

    Vec2f UILabel::getNaturalSize() const noexcept {
        return this->richTextFrame_.getNaturalSize();
    }

    const RichText& UILabel::getRichText() const noexcept {
        return this->richTextFrame_.getRichText();
    }

    void UILabel::setRichText(const RichText& richText) {
        this->richTextFrame_.setRichText(richText);
    }

    RichTextFrame& UILabel::getRichTextFrame() noexcept {
        return this->richTextFrame_;
    }

    const RichTextFrame& UILabel::getRichTextFrame() const noexcept {
        return this->richTextFrame_;
    }
}
