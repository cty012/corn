#include <corn/media/rich_text.h>
#include "font_impl.h"
#include "rich_text_impl.h"

namespace corn {
    TextStyle::TextStyle(const Font* font, size_t size)
        : font(font), size(size), color(Color::WHITE()), variant(FontVariant::REGULAR) {}

    TextStyle& TextStyle::setFont(const Font* newFont) {
        this->font = newFont;
        return *this;
    }

    TextStyle& TextStyle::setSize(size_t newSize) {
        this->size = newSize;
        return *this;
    }

    TextStyle& TextStyle::setColor(const Color& newColor) {
        this->color = newColor;
        return *this;
    }

    TextStyle& TextStyle::setVariant(FontVariant newVariant) {
        this->variant = newVariant;
        return *this;
    }

    RichText::Segment::Segment(const std::wstring& text, TextStyle style): text(), style(style) {
        this->text.setFont(style.font->sffont);
        this->text.setString(text);
        this->text.setCharacterSize(style.size);
        auto [r, g, b, a] = style.color.getRGBA();
        this->text.setFillColor(sf::Color(r, g, b, a));
        this->text.setStyle(sf::Text::Regular);  // TODO: convert to SFML style
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
