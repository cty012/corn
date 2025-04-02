#include <corn/util/string_utils.h>
#include "text_render_impl.h"

namespace corn {
    TextRenderImpl::Line::Line() : length(0), size(), precedeWithLineFeed(false) {}

    TextRenderImpl::TextRenderImpl(const RichText& richText) : lines(), limitWidth(false) {
        this->setRichText(richText, 0.0f);
    }

    void TextRenderImpl::setRichText(const RichText& richText, float padding) {
        this->original = richText;
        this->words.clear();
        for (const RichText::Segment& segment : richText.segments) {
            this->words.emplace_back(std::vector<std::u8string>(), segment.style);
            for (const std::u8string& word : breakIntoWords(segment.str)) {
                this->words.back().first.emplace_back(word);
            }
        }
        // Find natural width
        this->setWidth(-1.0f, padding);
        this->naturalSize = this->size;
    }

    void TextRenderImpl::setWidth(float width, float padding) {
        this->limitWidth = width >= 0;

        this->lines.clear();

        // Iterate over all words
        for (const auto& [segment, style] : this->words) {
            this->insertSegment(segment, style, width, padding);
        }

        // Find total size
        this->size = Vec2f::O();
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

    void TextRenderImpl::insertSegment(
            const std::vector<std::u8string>& segment, const TextStyle& style, float width, float padding) {

        // Current last line
        Line* currentLine = this->lines.empty() ? nullptr : &this->lines.back();

        // Buffer that stores the string to be pushed to the current line.
        std::u8string buffer;

        // Iterate over all words
        for (size_t i = 0; i < segment.size(); i++) {
            const std::u8string& word = segment[i];

            // Skip empty words (if any)
            if (word.empty()) continue;

            // If is line feed character
            if (word[0] == '\n') {
                // First push the current buffer to end of line
                TextRenderImpl::pushTextToLine(currentLine, buffer, style, padding);
                buffer = u8"";
                // Then start a new line
                currentLine = &this->lines.emplace_back();
                currentLine->size = Vec2f(0.0f, style.size * 1.2f + padding * 2);
                currentLine->precedeWithLineFeed = true;
                continue;
            }

            // If start of new line (but not preceded by line feed), skip pure space words
            bool notFirstLine = this->lines.size() > 1;
            bool currentLineEmpty = currentLine && currentLine->length == 0 && buffer.empty();
            if (notFirstLine && currentLineEmpty && !currentLine->precedeWithLineFeed && isWhitespace(getChar(word))) {
                continue;
            }

            // Measure width and height
            Vec2f newSize = measureTextSize(buffer + word, style);

            // Check if the word can fit
            if (currentLine && (!this->limitWidth || newSize.x + currentLine->size.x <= width)) {
                // 1. If the word can fit in current line, push word to buffer
                buffer += word;
            } else if (currentLineEmpty) {
                // 2. If the word cannot fit, but current line is empty
                // Use character wrap
                const char8_t* start = word.c_str();
                while (true) {
                    // Fit as many characters as possible
                    const char8_t* end = TextRenderImpl::insertCharsToEmptyLine(
                            currentLine, start, style, width, padding);
                    if (!*end) break;
                    // Wrap to next line
                    start = end;
                    currentLine = &this->lines.emplace_back();
                    currentLine->size = Vec2f(0.0f, style.size * 1.2f + padding * 2);
                    currentLine->precedeWithLineFeed = false;
                }
                // Put the remaining characters in the buffer
                buffer = std::u8string(start);
            } else {
                // 3. Otherwise, try to fit word in next line
                // Push original text to the back of the line
                TextRenderImpl::pushTextToLine(currentLine, buffer, style, padding);
                // Wrap to next line and clear buffer
                currentLine = &this->lines.emplace_back();
                currentLine->size = Vec2f(0.0f, style.size * 1.2f + padding * 2);
                currentLine->precedeWithLineFeed = false;
                buffer.clear();
                // Stay at current word
                i--;
            }
        }

        // If buffer not empty, push the buffer to the back of the current line
        TextRenderImpl::pushTextToLine(currentLine, buffer, style, padding);
    }

    void TextRenderImpl::pushTextToLine(
            TextRenderImpl::Line* line, const std::u8string& str, const TextStyle& style, float padding) {

        if (line && !str.empty()) {
            // Construct the text
            sf::Text text;
            setTextFont(text, style.font);
            setTextSize(text, style.size);
            setTextVariant(text, style.variant);
            setTextString(text, str);

            // Push the text to the back of the line
            line->contents.emplace_back(text, style.color);

            // Measure width and height
            sf::Rect<float> bounds = text.getLocalBounds();
            Vec2f textSize(bounds.width, bounds.height);

            // Add to total length & size
            line->length += count(str);
            line->size.x = textSize.x + line->size.x;
            line->size.y = std::max(line->size.y, textSize.y * 1.2f + padding * 2);
        }
    }

    const char8_t* TextRenderImpl::insertCharsToEmptyLine(
            TextRenderImpl::Line* line, const char8_t* word, const TextStyle& style, float width, float padding) {

        if (!*word) return word;

        // Set up SFML buffer text for measuring the width of buffer string.
        std::u8string buffer(word++, 1);
        bool firstChar = true;

        while (*word) {
            // Get a UTF-8 character from the remaining word
            std::u8string nextChar = getChar(word);

            // Measure width and height
            Vec2f newSize = measureTextSize(buffer + nextChar, style);

            if (firstChar || newSize.x <= width) {
                // If new character can fit
                buffer += nextChar;
                word += nextChar.size();
            } else {
                // If new character cannot fit
                // Push original text to the back of the line
                TextRenderImpl::pushTextToLine(line, buffer, style, padding);
                return word;
            }

            firstChar = false;
        }

        return word;
    }
}
