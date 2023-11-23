#pragma once

#include <SFML/Graphics.hpp>
#include <corn/media/rich_text.h>

namespace corn {
    struct RichText::Segment {
        sf::Text text;
        TextStyle style;
        Segment(const std::u8string& str, TextStyle style);
    };

    void setTextString(sf::Text& text, const std::u8string& str);
    void setTextFont(sf::Text& text, const Font* font);
    void setTextSize(sf::Text& text, unsigned int size);
    void setTextVariant(sf::Text& text, FontVariant variant);
}
