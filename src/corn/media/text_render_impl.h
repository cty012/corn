#pragma once

#include <string>
#include <utility>
#include <vector>
#include <SFML/Graphics.hpp>

namespace corn {
    class TextRender::TextRenderImpl {
    public:
        struct Line {
            std::vector<std::pair<sf::Text, Color>> contents;
            size_t length;
            Vec2 size;

            Line();
        };

        RichText original;
        std::vector<std::pair<std::vector<std::u8string>, TextStyle>> words;
        std::vector<Line> lines;
        bool limitWidth;
        Vec2 size;

        /// @brief Constructor.
        explicit TextRenderImpl(const RichText& richText);

        /**
         * @brief Setter for the rich text. Will reset all cached information and recalculate words.
         * @param richText The new rich text.
         */
        void setRichText(const RichText& richText);

        /**
         * @brief Force reset the width of the text.
         * @param width New width of the text. If negative, width limit will be removed.
         */
        void setWidth(float width);

    private:
        /**
         * @brief Helper function. Inserts a segment of the text into the lines respecting the width limit.
         * @param segment The segment broken into words.
         * @param style The style of the segment.
         * @param width The width to fit in.
         */
        void insertSegment(const std::vector<std::u8string>& segment, const TextStyle& style, float width);

        /**
         * @brief Helper function. Inserts
         * @param word
         * @param style
         * @param width
         * @return
         */
        static const char8_t* insertCharsToEmptyLine(const char8_t* word, Line* line, const TextStyle& style, float width);
    };

    void setTextString(sf::Text& text, const std::u8string& str);

    void setTextFont(sf::Text& text, const Font* font);

    void setTextSize(sf::Text& text, unsigned int size);

    void setTextVariant(sf::Text& text, FontVariant variant);
}
