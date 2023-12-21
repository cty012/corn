#include <corn/media/text_render.h>
#include <corn/util/string_utils.h>
#include "font_impl.h"
#include "text_render_impl.h"

namespace corn {
    TextRender::TextRenderImpl::TextRenderImpl(const RichText& richText) : lines(), limitWidth(false) {
        this->setRichText(richText);
    }

    void TextRender::TextRenderImpl::setRichText(const RichText& richText) {
        this->original = richText;
        this->words.clear();
        for (const RichText::Segment& segment : richText.segments) {
            this->words.emplace_back(std::vector<std::u8string>(), segment.style);
            for (const std::u8string& word : breakIntoWords(segment.str)) {
                this->words.back().first.emplace_back(word);
            }
        }
    }

    void TextRender::TextRenderImpl::setWidth(float width) {
        this->limitWidth = width >= 0;
        printf("Limit width: %d\n", this->limitWidth);

        this->lines.clear();
        Line* currentLine = &this->lines.emplace_back();
        currentLine->length = 0;
        currentLine->size = Vec2::ZERO();

        // Whether started a new line, the first line doesn't count
        bool startOfNewLine = false;

        // Iterate over all words
        for (const auto& [segment, style] : this->words) {
            for (const std::u8string& word: segment) {  // todo
                // Skip empty words (if any)
                if (word.empty()) {
                    continue;
                }

                // If is line feed character, start a new line
                if (word[0] == '\n') {
                    currentLine = &this->lines.emplace_back();
                    currentLine->length = 0;
                    currentLine->size = Vec2::ZERO();
                    startOfNewLine = true;
                    continue;
                }

                // If start of new line, skip pure space words
                if (startOfNewLine && isWhitespace(getChar(word))) {
                    continue;
                }

                // Measure width
                sf::Text text;
                setTextFont(text, style.font);
                setTextSize(text, (unsigned int) style.size);
                setTextVariant(text, style.variant);
                setTextString(text, word);
                sf::Rect<float> bounds = text.getLocalBounds();
                Vec2 newSize = {bounds.width, bounds.height};

                // TODO: character wrap on word too long
                float newWidth = newSize.x + currentLine->size.x;
                if (currentLine->length == 0 || !this->limitWidth || newWidth <= width) {
                    // If the word can fit in current line
                    // Push the word to the back of the line
                    currentLine->contents.emplace_back(text, style.color);
                    startOfNewLine = false;

                    // Add to total length & size
                    currentLine->length += count(word);
                    currentLine->size.x = newWidth;
                    currentLine->size.y = std::max(currentLine->size.y, newSize.y);
                } else {
                    // If the word cannot fit in current line
                    // Wrap to next line
                    currentLine = &this->lines.emplace_back();
                    currentLine->length = 0;
                    currentLine->size = Vec2::ZERO();
                    startOfNewLine = true;

                    // Add word to new line if is not whitespace
                    if (!isWhitespace(getChar(word))) {
                        currentLine->contents.emplace_back(text, style.color);
                        startOfNewLine = false;

                        // Add to total length & size
                        currentLine->length += count(word);
                        currentLine->size.x += newSize.x;
                        currentLine->size.y = std::max(currentLine->size.y, newSize.y);
                    }
                }
            }
        }

        // Check if the last line is empty
        if (this->lines.back().length == 0) {
            this->lines.pop_back();
        }

        // Find total size
        this->size = Vec2::ZERO();
        if (this->limitWidth) {
            this->size.x = width;
            for (Line& line : this->lines) {
                this->size.y += line.size.y;
            }
        } else {
            for (Line& line : this->lines) {
                this->size.x = std::max(this->size.x, line.size.x);
                this->size.y += line.size.y;
            }
        }
    }

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
        this->impl_->setRichText(richText);
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
        this->impl_->setWidth(std::max(width, 0.0f));
    }

    void TextRender::setWidthNoLimit() {
        this->impl_->setWidth(-1.0f);
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
