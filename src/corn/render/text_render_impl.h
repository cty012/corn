#pragma once

#include <string>
#include <utility>
#include <vector>
#include <corn/geometry/vec.h>
#include <corn/util/color.h>
#include <corn/util/rich_text.h>

namespace corn {
    class TextRenderImpl {
    public:
        struct Line {
            std::vector<std::pair<int, Color>> contents;
            size_t length;
            Vec2f size;
            bool precedeWithLineFeed;

            Line();
        };

        RichText original;
        std::vector<std::pair<std::vector<std::u8string>, TextStyle>> words;
        std::vector<Line> lines;
        bool limitWidth;
        Vec2f naturalSize;
        Vec2f size;

        explicit TextRenderImpl(const RichText& richText);
        void setRichText(const RichText& richText, float padding);
        void setWidth(float width, float padding);
    };


    // class TextRenderImpl {
    // public:
    //     struct Line {
    //         std::vector<std::pair<sf::Text, Color>> contents;
    //         size_t length;
    //         Vec2f size;
    //         bool precedeWithLineFeed;
    //
    //         Line();
    //     };
    //
    //     RichText original;
    //     std::vector<std::pair<std::vector<std::u8string>, TextStyle>> words;
    //     std::vector<Line> lines;
    //     bool limitWidth;
    //     Vec2f naturalSize;
    //     Vec2f size;
    //
    //     /// @brief Constructor.
    //     explicit TextRenderImpl(const RichText& richText);
    //
    //     /**
    //      * @brief Setter for the rich text. Will reset all cached information and recalculate words.
    //      * @param richText The new rich text.
    //      */
    //     void setRichText(const RichText& richText, float padding);
    //
    //     /**
    //      * @brief Force reset the width of the text.
    //      * @param width New width of the text. If negative, width limit will be removed.
    //      * @param padding The padding above and below the line.
    //      */
    //     void setWidth(float width, float padding);
    //
    // private:
    //     /**
    //      * @brief Helper function. Inserts a segment of the text into the lines respecting the width limit.
    //      * @param segment The segment broken into words.
    //      * @param style The style of the segment.
    //      * @param width The width limit.
    //      * @param padding The padding above and below each line.
    //      */
    //     void insertSegment(const std::vector<std::u8string>& segment, const TextStyle& style, float width, float padding);
    //
    //     /**
    //      * @brief Helper function. Push the text to the back of the line.
    //      * @param line The target line to push to.
    //      * @param str The UTF-8 string to render.
    //      * @param style The style of the text.
    //      * @param padding The padding above and below the line.
    //      *
    //      * This function assumes that the current line can fit the text.
    //      */
    //     static void pushTextToLine(TextRenderImpl::Line* line, const std::u8string& str, const TextStyle& style, float padding);
    //
    //     /**
    //      * @brief Helper function. Inserts as many characters as possible to the given line.
    //      * @param word UTF-8 word to insert, as a C style string. Must not be empty.
    //      * @param line The target line to push to. Assumed to be empty.
    //      * @param style The style of the word.
    //      * @param width The width limit.
    //      * @param padding The padding above and below the line.
    //      * @return Points to the start of the remaining characters.
    //      *
    //      * Only use this function if the word is too long to fit in the current
    //      */
    //     static const char8_t* insertCharsToEmptyLine(Line* line, const char8_t* word, const TextStyle& style, float width, float padding);
    // };
    //
    // Vec2f measureTextSize(const std::u8string& str, const TextStyle& style);
    //
    // void setTextString(sf::Text& text, const std::u8string& str);
    //
    // void setTextFont(sf::Text& text, const Font* font);
    //
    // void setTextSize(sf::Text& text, float size);
    //
    // void setTextVariant(sf::Text& text, FontVariant variant);
}
