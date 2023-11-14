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

    float UILabel::getNaturalWidth() const {
        float width = 0.0f;
        for (RichText::Segment* segment : this->text.segments) {
            width += segment->text.getLocalBounds().width;
        }
        return width;
    }

    float UILabel::getNaturalHeight() const {
        float height = 0.0f;
        for (RichText::Segment* segment : this->text.segments) {
            height = std::max(height, segment->text.getLocalBounds().height);
        }
        return height;
    }
}
