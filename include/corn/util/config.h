#pragma once

#include <string>
#include <corn/util/color.h>

namespace corn {
    /**
     * @enum DisplayMode
     * @brief How the window will be displayed on the computer screen.
     */
    enum class DisplayMode {
        WINDOWED,
        WINDOWED_FIXED,               // Not resizable
        FULLSCREEN,
        // BORDERLESS,
    };

    /**
     * @class Config
     * @brief Configurations for the game.
     */
    class Config {
    public:
        size_t width = 1280;                                   // Window width (in pixels)
                                                               // @todo If set to 0, will use screen width
        size_t height = 720;                                   // Window height (in pixels)
                                                               // @todo If set to 0, will use screen height
        std::string title = "My Corn Game";                    // Game title displayed at the top of the window
        Color background = Color::rgb(0, 0, 0);       // Background color of the game window
        DisplayMode mode = DisplayMode::WINDOWED;              // Display mode
        int antialiasing = 1;                                  // Antialiasing level (power of 2, depends on hardware)
    };
}
