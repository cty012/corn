#include <corn/media/text_render.h>
#include <corn/util/string_utils.h>
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

    /**
     * @param str Target string.
     * @return The next word in the string.
     */
    const char8_t* getWord(const char8_t* str) {
        while (*str && (*str != '\n')) {
            std::u8string nextChar = getChar(str);
            if (isWhitespace(nextChar)) break;
            str += nextChar.size();
        }
        while (*str && (*str != '\n')) {
            std::u8string nextChar = getChar(str);
            if (!isWhitespace(nextChar)) break;
            str += nextChar.size();
        }
        return str;
    }

    /**
     * @brief Fits a string into a specified width, wraps on whitespaces.
     * @param target The sf::Text to store the results.
     * @param str The target string to fit.
     * @param width The specified width. A negative value means no width limit.
     * @param fitAtLeastOneWord Whether to avoid fitting an empty string.
     * @return Whether a line break is required.
     *
     * The function writes the result to `target` and updates `str` to the start of the next word (or the end of the
     * string if there is no next word).
     *
     * Note the the only case that `false` is returned is when the entire string is fit in.
     */
    bool getLine(sf::Text& target, const char8_t*& str, float width, bool fitAtLeastOneWord) {
        // Current string fitting in the width
        std::u8string current;
        setTextString(target, current);

        while (*str && (*str != '\n')) {
            // Get the next word
            const char8_t* end = getWord(str);
            std::u8string next = current + std::u8string(str, end);
            setTextString(target, next);

            // If exceed width limit
            if (width >= 0 && target.getLocalBounds().width > width) {
                if (fitAtLeastOneWord && current.empty()) {
                    str = end;
                } else {
                    setTextString(target, current);
                }
                return true;
            }

            // If not exceed width limit
            str = end;
            current = next;
        }

        // If *str is \n, needs to change line
        return *str;
    }

    void TextRender::TextRenderImpl::setWidth(float width) {
        this->limitWidth = width >= 0;

        this->lines.clear();
        Line& firstLine = this->lines.emplace_back();
        firstLine.length = 0;
        firstLine.size = Vec2::ZERO();

        // Iterate over all segments
        for (const RichText::Segment& segment : this->original.segments) {
            // Skip empty segments
            if (segment.str.empty()) continue;
            const char8_t* str = segment.str.c_str();
            Line& currentLine = this->lines.back();

            float remainWidth = width;
            while (*str) {
                // Check if first word
                bool firstWord = currentLine.length == 0;

                // Add new segment
                auto& [text, color] = currentLine.contents.emplace_back();
                color = segment.style.color;
                setTextFont(text, segment.style.font);
                setTextSize(text, (unsigned int) segment.style.size);
                setTextVariant(text, segment.style.variant);

                // Get line
                const char8_t* begin = str;
                bool needLineBreak = getLine(text, str, remainWidth, firstWord);

                // Add to total length & size
                currentLine.length += count({ begin, str });
                sf::Rect<float> bounds = text.getLocalBounds();
                Vec2 newSize = { bounds.width, bounds.height };
                currentLine.size.x += newSize.x;
                currentLine.size.y = std::max(currentLine.size.y, newSize.y);

                // Remove newSize.width from remainWidth
                if (this->limitWidth) {
                    remainWidth -= newSize.x;
                }

                // If linebreak, add new line and remove preceding spaces
                if (needLineBreak) {
                    currentLine = this->lines.emplace_back();
                    currentLine.length = 0;
                    currentLine.size = Vec2::ZERO();
                    // todo: remove preceding spaces
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

    void TextRender::setWidth(float width) {
        this->impl_->setWidth(std::max(width, 0.0f));
    }

    void TextRender::setWidthNoLimit() {
        this->impl_->setWidth(-1.0f);
//        this->impl_->limitWidth = false;
//
//        this->impl_->lines.clear();
//        this->impl_->lines.emplace_back();
//        TextRenderImpl::Line& firstLine = this->impl_->lines[0];
//        firstLine.length = 0;
//        firstLine.size = { 0.0f, 0.0f };
//
//        // Add all segments to the first line
//        for (const RichText::Segment& segment : this->impl_->original.segments) {
//            auto& [text, color] = firstLine.contents.emplace_back();
//            color = segment.style.color;
//            // Count characters
//            firstLine.length += count(segment.str);
//            // Set styles
//            setTextFont(text, segment.style.font);
//            setTextString(text, segment.str);
//            setTextSize(text, (unsigned int) segment.style.size);
//            setTextVariant(text, segment.style.variant);
//            // Calculate size
//            firstLine.size.x += text.getLocalBounds().width;
//            firstLine.size.y = std::max(firstLine.size.y, text.getLocalBounds().height);
//        }
//
//        // Calculate size
//        this->impl_->size = firstLine.size;
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
