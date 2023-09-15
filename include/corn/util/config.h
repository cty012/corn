#pragma once

#include <string>
#include <corn/util/color.h>

namespace corn {
    enum class DisplayMode {
        WINDOWED, FULLSCREEN, // BORDERLESS
    };

    class Config {
    public:
        size_t width = 1280;
        size_t height = 720;
        std::string title = "My CORN Game";
        Color background = Color::rgb(60, 179, 113);
        DisplayMode mode = DisplayMode::WINDOWED;
    };
}
