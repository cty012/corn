#include <corn/util/rich_text.h>

namespace corn {
    TextStyle::TextStyle(const Font* font, size_t size) noexcept
            : font(font), size(size), color(Color::WHITE()), variant(FontVariant::REGULAR) {}

    TextStyle::TextStyle(const Font* font, size_t size, const Color& color, FontVariant variant) noexcept
            : font(font), size(size), color(color), variant(variant) {}

    TextStyle TextStyle::setFont(const Font* newFont) const noexcept {
        TextStyle style = *this;
        style.font = newFont;
        return style;
    }

    TextStyle TextStyle::setSize(size_t newSize) const noexcept {
        TextStyle style = *this;
        style.size = newSize;
        return style;
    }

    TextStyle TextStyle::setColor(const Color& newColor) const noexcept {
        TextStyle style = *this;
        style.color = newColor;
        return style;
    }

    TextStyle TextStyle::setVariant(FontVariant newVariant) const noexcept {
        TextStyle style = *this;
        style.variant = newVariant;
        return style;
    }

    RichText::RichText() noexcept : segments() {}

    RichText::~RichText() noexcept = default;

    RichText& RichText::addText(const std::u8string& text, TextStyle style) noexcept {
        this->segments.emplace_back(text, style);
        return *this;
    }
}
