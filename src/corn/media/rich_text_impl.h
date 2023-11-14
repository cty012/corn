#pragma once

#include <SFML/Graphics.hpp>
#include <corn/media/rich_text.h>

namespace corn {
    struct RichText::Segment {
        sf::Text text;
        TextStyle style;
        Segment(const std::u8string& text, TextStyle style);
    };
}
