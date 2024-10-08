#include <corn/util/rich_text.h>

#include <utility>

namespace corn {
    TextStyle::TextStyle(const Font* font, float size) noexcept
            : font(font), size(size), color(Color::WHITE()), variant(FontVariant::REGULAR) {}

    TextStyle::TextStyle(const Font* font, float size, const Color& color, FontVariant variant) noexcept
            : font(font), size(size), color(color), variant(variant) {}

    TextStyle TextStyle::setFont(const Font* newFont) const noexcept {
        TextStyle style = *this;
        style.font = newFont;
        return style;
    }

    TextStyle TextStyle::setSize(float newSize) const noexcept {
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

    RichText::Segment::Segment(std::u8string str, TextStyle style) noexcept : str(std::move(str)), style(style) {}

    RichText::RichText() noexcept : segments() {}

    RichText::~RichText() noexcept = default;

    RichText& RichText::addText(const std::u8string& text, TextStyle style) noexcept {
        this->segments.emplace_back(text, style);
        return *this;
    }

    RichText& RichText::addText(const std::string& text, TextStyle style) noexcept {
        this->segments.emplace_back(std::u8string((const char8_t*)text.c_str()), style);
        return *this;
    }
}
