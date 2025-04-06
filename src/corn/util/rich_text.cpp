#include <corn/util/rich_text.h>

#include <utility>

namespace corn {
    TextStyle::TextStyle(const Font* font, float size, Color color) noexcept
            : font(font), size(size), color(std::move(color)),
              weight(FONT_WEIGHT_REGULAR), italic(false), underline(false), position(FontPosition::REGULAR) {}

    TextStyle::TextStyle(
            const Font* font, float size, Color color,
            float weight, bool italic, bool underline, FontPosition position) noexcept
            : font(font), size(size), color(std::move(color)),
              weight(weight), italic(italic), underline(underline), position(position) {}

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
        style.color = std::move(newColor);
        return style;
    }

    TextStyle TextStyle::setWeight(float newWeight) const noexcept {
        TextStyle style = *this;
        style.weight = newWeight;
        return style;
    }

    TextStyle TextStyle::setItalic(bool newItalic) const noexcept {
        TextStyle style = *this;
        style.italic = newItalic;
        return style;
    }

    TextStyle TextStyle::setUnderline(bool newUnderline) const noexcept {
        TextStyle style = *this;
        style.underline = newUnderline;
        return style;
    }

    TextStyle TextStyle::setPosition(FontPosition newPosition) const noexcept {
        TextStyle style = *this;
        style.position = newPosition;
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

    bool operator==(const TextStyle& lhs, const TextStyle& rhs) noexcept {
        return lhs.font == rhs.font &&
               lhs.size == rhs.size &&
               lhs.color == rhs.color &&
               lhs.weight == rhs.weight &&
               lhs.italic == rhs.italic &&
               lhs.underline == rhs.underline &&
               lhs.position == rhs.position;
    }

    bool operator!=(const TextStyle& lhs, const TextStyle& rhs) noexcept {
        return !(lhs == rhs);
    }

    bool operator==(const RichText::Segment& lhs, const RichText::Segment& rhs) noexcept {
        return lhs.text == rhs.text && lhs.style == rhs.style;
    }

    bool operator!=(const RichText::Segment& lhs, const RichText::Segment& rhs) noexcept {
        return !(lhs == rhs);
    }

    bool operator==(const RichText& lhs, const RichText& rhs) noexcept {
        return lhs.segments == rhs.segments && lhs.textAlign == rhs.textAlign;
    }

    bool operator!=(const RichText& lhs, const RichText& rhs) noexcept {
        return !(lhs == rhs);
    }
}
