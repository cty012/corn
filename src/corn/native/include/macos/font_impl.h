#pragma once

#include <filesystem>
#include <string>
#include <CoreText/CoreText.h>

namespace corn {
    /**
     * @class FontState
     * @brief Loading status of the font.
     */
    enum class FontState {
        LOADING, LOADED, LOAD_FAILED
    };

    class Font {
    public:
        FontState state;
        CGFontRef cgFont;

        Font();

        ~Font();

        static Font* createFromSystem(const std::string& name);

        static Font* createFromPath(const std::filesystem::path& path);

        void destroy();
    };
}
