#include <corn/util/rich_text.h>

#include <utility>

namespace corn {
    TextStyle::TextStyle(const Font* font, float size, Color color) noexcept
            : font(font), size(size), color(std::move(color)),
              weight(FontWeight::REGULAR), slant(FontSlant::REGULAR), decoration(FontDecoration::REGULAR) {}

    TextStyle::TextStyle(
            const Font* font, float size, Color color,
            FontWeight weight, FontSlant slant, FontDecoration decoration) noexcept
            : font(font), size(size), color(std::move(color)), weight(weight), slant(slant), decoration(decoration) {}

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

    TextStyle TextStyle::setColor(Color newColor) const noexcept {
        TextStyle style = *this;
        style.color = newColor;
        return style;
    }

    TextStyle TextStyle::setWeight(FontWeight newWeight) const noexcept {
        TextStyle style = *this;
        style.weight = newWeight;
        return style;
    }

    TextStyle TextStyle::setSlant(FontSlant newSlant) const noexcept {
        TextStyle style = *this;
        style.slant = newSlant;
        return style;
    }

    TextStyle TextStyle::setDecoration(FontDecoration newDecoration) const noexcept {
        TextStyle style = *this;
        style.decoration = newDecoration;
        return style;
    }

    RichText::Segment::Segment(std::string text, TextStyle style) noexcept
            : text(std::move(text)), style(std::move(style)) {}

    RichText::RichText() noexcept : segments() {}

    RichText::~RichText() noexcept = default;

    RichText& RichText::addText(std::string text, TextStyle style) noexcept {
        this->segments.emplace_back(std::move(text), std::move(style));
        return *this;
    }

    std::string RichText::getString() const noexcept {
        std::string result;
        for (const Segment& segment : this->segments) {
            result += segment.text;
        }
        return result;
    }
}
