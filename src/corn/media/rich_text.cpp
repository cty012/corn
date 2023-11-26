#include <corn/media/rich_text.h>
#include "font_impl.h"
#include "rich_text_impl.h"

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

    RichText::Segment::Segment(const std::u8string& str, TextStyle style) : text(), style(style) {
        setTextFont(this->text, style.font);
        setTextString(this->text, str);
        setTextSize(this->text, (unsigned int)style.size);
        setTextVariant(this->text, style.variant);
    }

    void setTextString(sf::Text& text, const std::u8string& str) {
        text.setString(sf::String::fromUtf8(str.begin(), str.end()));
    }

    void setTextFont(sf::Text& text, const Font* font) {
        text.setFont(font->sffont);
    }

    void setTextSize(sf::Text& text, unsigned int size) {
        text.setCharacterSize(size);
    }

    void setTextVariant(sf::Text& text, FontVariant variant) {
        switch (variant) {
            case FontVariant::REGULAR:
                text.setStyle(sf::Text::Regular);
                break;
            case FontVariant::BOLD:
                text.setStyle(sf::Text::Bold);
                break;
            case FontVariant::ITALIC:
                text.setStyle(sf::Text::Italic);
                break;
            case FontVariant::UNDERLINE:
                text.setStyle(sf::Text::Underlined);
                break;
        }
    }

    RichText::RichText() : segments() {}

    RichText::~RichText() {
        for (Segment* segment : this->segments) {
            delete segment;
        }
    }

    RichText::RichText(const RichText& other) : segments() {
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

    RichText& RichText::addText(const std::u8string& text, TextStyle style) {
        this->segments.push_back(new Segment(text, style));
        return *this;
    }

    const TextStyle& getStyle(RichText::Segment* segment) {
        return segment->style;
    }

    void setString(RichText::Segment* segment, const std::u8string& str) {
        setTextString(segment->text, str);
    }

    void setFont(RichText::Segment* segment, const Font* font) {
        segment->style.font = font;
        setTextFont(segment->text, font);
    }

    void setSize(RichText::Segment* segment, unsigned int size) {
        segment->style.size = size;
        setTextSize(segment->text, size);
    }

    void setColor(RichText::Segment* segment, Color color) {
        segment->style.color = color;
    }

    void setVariant(RichText::Segment* segment, FontVariant variant) {
        segment->style.variant = variant;
        setTextVariant(segment->text, variant);
    }
}
