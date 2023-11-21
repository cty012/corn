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

    Vec2 UILabel::getNaturalSize() const {
        float nw = 0.0f, nh = 0.0f;
        for (RichText::Segment* segment: this->text.segments) {
            nw += segment->text.getLocalBounds().width;
            nh = std::max(nh, segment->text.getLocalBounds().height);
        }
        return { nw, nh };
    }
}
