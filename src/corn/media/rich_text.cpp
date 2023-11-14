#include <corn/media/rich_text.h>
#include "font_impl.h"
#include "rich_text_impl.h"

namespace corn {
    TextStyle::TextStyle(const Font* font, size_t size)
        : font(font), size(size), color(Color::WHITE()), variant(FontVariant::REGULAR) {}

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

    RichText::Segment::Segment(const std::wstring& text, TextStyle style): text(), style(style) {
        this->text.setFont(style.font->sffont);
        this->text.setString(text);
        this->text.setCharacterSize((unsigned int)style.size);
        auto [r, g, b, a] = style.color.getRGBA();
        this->text.setFillColor(sf::Color(r, g, b, a));
        switch (style.variant) {
            case FontVariant::REGULAR:
                this->text.setStyle(sf::Text::Regular);
                break;
            case FontVariant::BOLD:
                this->text.setStyle(sf::Text::Bold);
                break;
            case FontVariant::ITALIC:
                this->text.setStyle(sf::Text::Italic);
                break;
            case FontVariant::UNDERLINE:
                this->text.setStyle(sf::Text::Underlined);
                break;
        }
    }

    RichText::RichText(): segments() {}

    RichText::~RichText() {
        for (Segment* segment : this->segments) {
            delete segment;
        }
    }

    RichText::RichText(const RichText& other): segments() {
        this->segments.reserve(other.segments.size());
        for (Segment* segment : other.segments) {
            this->segments.push_back(new Segment(*segment));
        }
    }

    RichText& RichText::operator=(const RichText& other) {
        if (&other == this) return *this;
        for (Segment* segment : this->segments) {
            delete segment;
        }
        this->segments.clear();
        this->segments.reserve(other.segments.size());
        for (Segment* segment : other.segments) {
            this->segments.push_back(new Segment(*segment));
        }
        return *this;
    }

    RichText::RichText(RichText&& other) noexcept : segments(std::move(other.segments)) {
        other.segments.clear();
    }

    RichText& RichText::operator=(RichText&& other) noexcept {
        if (&other == this) return *this;
        for (Segment* segment : this->segments) {
            delete segment;
        }
        this->segments.clear();
        this->segments = std::move(other.segments);
        other.segments.clear();
        return *this;
    }

    RichText& RichText::addText(const std::wstring& text, TextStyle style) {
        this->segments.push_back(new Segment(text, style));
        return *this;
    }
}
