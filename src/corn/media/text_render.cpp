#include <corn/media/text_render.h>
#include <corn/util/utf8.h>
#include "font_impl.h"
#include "text_render_impl.h"

namespace corn {
    TextRender::TextRenderImpl::TextRenderImpl(const RichText& richText) : original(richText), lines(), limitWidth() {}

    TextRender::TextRender(const RichText& richText) : impl_(new TextRenderImpl(richText)) {
        this->setWidthNoLimit();
    }

    TextRender::TextRender(const RichText& richText, float width) : impl_(new TextRenderImpl(richText)) {
        this->setWidth(width);
    }

    const RichText& TextRender::getText() const {
        return this->impl_->original;
    }

    void TextRender::setText(const RichText& richText) {
        this->impl_->original = richText;
        if (this->impl_->limitWidth) {
            this->setWidth(this->impl_->size.x);
        } else {
            this->setWidthNoLimit();
        }
    }

    const Vec2& TextRender::getSize() const {
        return this->impl_->size;
    }

    void TextRender::setWidth(float width) {
        (void)width;
        // TODO
    }

    void TextRender::setWidthNoLimit() {
        this->impl_->limitWidth = false;

        this->impl_->lines.clear();
        this->impl_->lines.emplace_back();
        TextRenderImpl::Line& firstLine = this->impl_->lines[0];
        firstLine.length = 0;
        firstLine.size = { 0.0f, 0.0f };

        // Add all segments to the first line
        for (const RichText::Segment& segment : this->impl_->original.segments) {
            firstLine.contents.emplace_back();
            auto& [text, color] = firstLine.contents.back();
            color = segment.style.color;
            // Count characters
            firstLine.length += count(segment.str);
            // Set styles
            setTextFont(text, segment.style.font);
            setTextString(text, segment.str);
            setTextSize(text, (unsigned int) segment.style.size);
            setTextVariant(text, segment.style.variant);
            // Calculate size
            firstLine.size.x += text.getLocalBounds().width;
            firstLine.size.y = std::max(firstLine.size.y, text.getLocalBounds().height);
        }

        // Calculate size
        this->impl_->size = firstLine.size;
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
}
