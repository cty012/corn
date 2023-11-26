#include <corn/util/rich_text.h>

namespace corn {
    TextStyle::TextStyle(const Font* font, size_t size)
            : font(font), size(size), color(Color::WHITE()), variant(FontVariant::REGULAR) {}

    TextStyle::TextStyle(const Font* font, size_t size, const Color& color, FontVariant variant)
            : font(font), size(size), color(color), variant(variant) {}

    TextStyle TextStyle::setFont(const Font* newFont) {
        TextStyle style = *this;
        style.font = newFont;
        return style;
    }

    TextStyle TextStyle::setSize(size_t newSize) {
        TextStyle style = *this;
        style.size = newSize;
        return style;
    }

    TextStyle TextStyle::setColor(const Color& newColor) {
        TextStyle style = *this;
        style.color = newColor;
        return style;
    }

    TextStyle TextStyle::setVariant(FontVariant newVariant) {
        TextStyle style = *this;
        style.variant = newVariant;
        return style;
    }

    RichText::RichText() : segments() {}

    RichText::~RichText() = default;

    RichText& RichText::addText(const std::u8string& text, TextStyle style) {
        this->segments.emplace_back(text, style);
        return *this;
    }
}
