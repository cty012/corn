#pragma once

#include <SFML/Graphics.hpp>
#include <corn/media/rich_text.h>

namespace corn {
    class RichText::Segment {
    public:
        Segment(const std::wstring& text, TextStyle style);

    private:
        sf::Text text;
        TextStyle style;
    };
}
