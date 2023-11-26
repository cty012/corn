#pragma once

#include <string>
#include <utility>
#include <vector>
#include <SFML/Graphics.hpp>
#include <corn/media/text_render.h>

namespace corn {
    class TextRender::TextRenderImpl {
    public:
        struct Line {
            std::vector<std::pair<sf::Text, Color>> contents;
            size_t length;
            Vec2 size;
        };

        RichText original;
        std::vector<Line> lines;
        bool limitWidth;
        Vec2 size;

        explicit TextRenderImpl(const RichText& richText);
    };

    void setTextString(sf::Text& text, const std::u8string& str);
    void setTextFont(sf::Text& text, const Font* font);
    void setTextSize(sf::Text& text, unsigned int size);
    void setTextVariant(sf::Text& text, FontVariant variant);
}
