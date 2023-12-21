#include <corn/media/text_render.h>
#include <corn/util/string_utils.h>
#include "font_impl.h"
#include "text_render_impl.h"

namespace corn {
    TextRender::TextRenderImpl::Line::Line() : length(0), size() {}

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

        // Iterate over all words
        for (const auto& [segment, style] : this->words) {
            this->insertSegment(segment, style, width);
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

    void TextRender::TextRenderImpl::insertSegment(
            const std::vector<std::u8string>& segment, const TextStyle& style, float width) {

        // Current line to insert to
        Line* currentLine = this->lines.empty() ? nullptr : &this->lines.back();

        // Set up SFML buffer text for measuring the width of buffer string.
        std::u8string bufferString, newBufferString;
        sf::Text bufferText, newBufferText;
        setTextFont(bufferText, style.font);
        setTextSize(bufferText, (unsigned int) style.size);
        setTextVariant(bufferText, style.variant);
        setTextString(bufferText, u8"");
        setTextFont(newBufferText, style.font);
        setTextSize(newBufferText, (unsigned int) style.size);
        setTextVariant(newBufferText, style.variant);

        // Iterate over all words
        for (size_t i = 0; i < segment.size(); i++) {
            const std::u8string& word = segment[i];

            // Skip empty words (if any)
            if (word.empty()) continue;

            // If is line feed character
            if (word[0] == '\n') {
                // First push the current buffer to end of line
                if (currentLine && !bufferString.empty()) {
                    currentLine->contents.emplace_back(bufferText, style.color);
                }
                // Then start a new line
                currentLine = &this->lines.emplace_back();
                continue;
            }

            // If start of new line, skip pure space words
            bool notFirstLine = this->lines.size() > 1;
            bool currentLineEmpty = currentLine && currentLine->length == 0 && bufferString.empty();
            if (notFirstLine && currentLineEmpty && isWhitespace(getChar(word))) {
                continue;
            }

            // Measure width and height
            newBufferString = bufferString + word;
            setTextString(newBufferText, newBufferString);
            sf::Rect<float> newBounds = newBufferText.getLocalBounds();
            Vec2 newSize = { newBounds.width, newBounds.height };

            // Check if the word can fit
            if (currentLine && (!this->limitWidth || newSize.x + currentLine->size.x <= width)) {
                // 1. If the word can fit in current line
                // Push the new text to the back of the line
                currentLine->contents.emplace_back(newBufferText, style.color);
                // Add to total length & size
                currentLine->length += count(newBufferString);
                currentLine->size.x = newSize.x + currentLine->size.x;
                currentLine->size.y = std::max(currentLine->size.y, newSize.y);
                // Update buffer
                bufferString = std::move(newBufferString);
                bufferText = std::move(newBufferText);
            } else if (currentLineEmpty) {
                // 2. If the word cannot fit, but current line is empty
                const char8_t* start = word.c_str();
                while (true) {
                    start = corn::TextRender::TextRenderImpl::insertCharsToEmptyLine(start, currentLine, style, width);
                    if (!start) break;
                    // Wrap to next line
                    currentLine = &this->lines.emplace_back();
                }
                // Put the remaining characters in the buffer
                bufferString = std::u8string(start);
                setTextString(bufferText, bufferString);

            } else {
                // 3. Otherwise, push the original text in current line and change line. Do not move forward.
                // Push original text to the back of the line
                if (currentLine && !bufferString.empty()) {
                    currentLine->contents.emplace_back(bufferText, style.color);
                    // Add to total length & size
                    currentLine->length += count(bufferString);
                    currentLine->size.x = newSize.x + currentLine->size.x;
                    currentLine->size.y = std::max(currentLine->size.y, newSize.y);
                }
                // Wrap to next line
                currentLine = &this->lines.emplace_back();
                // Update buffer
                bufferString.clear();
                bufferText = sf::Text();
                // Do not move forward
                i--;
            }
        }

        // If buffer not empty, push the buffer to the back of the current line
        if (currentLine && !bufferString.empty()) {
            currentLine->contents.emplace_back(bufferText, style.color);
        }
    }

    const char8_t* TextRender::TextRenderImpl::insertCharsToEmptyLine(
            const char8_t* word, TextRender::TextRenderImpl::Line* line, const TextStyle& style, float width) {

        // Set up SFML buffer text for measuring the width of buffer string.
        std::u8string bufferString, newBufferString;
        sf::Text bufferText, newBufferText;
        setTextFont(bufferText, style.font);
        setTextSize(bufferText, (unsigned int) style.size);
        setTextVariant(bufferText, style.variant);
        setTextString(bufferText, u8"");
        setTextFont(newBufferText, style.font);
        setTextSize(newBufferText, (unsigned int) style.size);
        setTextVariant(newBufferText, style.variant);

        while (*word) {
            // Get a UTF-8 character from the remaining word
            std::u8string nextChar = getChar(word);
            newBufferString += nextChar;

            // Measure width and height
            setTextString(newBufferText, newBufferString);
            sf::Rect<float> newBounds = newBufferText.getLocalBounds();
            Vec2 newSize = { newBounds.width, newBounds.height };

            if (newSize.x <= width) {
                // If new character can fit
                bufferString = std::move(newBufferString);
                bufferText = std::move(newBufferText);
                word += nextChar.size();
            } else {
                // If new character cannot fit
                // Push original text to the back of the line
                if (line && !bufferString.empty()) {
                    line->contents.emplace_back(bufferText, style.color);
                    // Set total length & size
                    line->length = count(bufferString);
                    line->size = newSize;
                }
                return word;
            }
        }

        return nullptr;
    }
}
