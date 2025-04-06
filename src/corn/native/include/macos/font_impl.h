#pragma once

#include <filesystem>
#include <string>
#include <CoreText/CoreText.h>
#include <corn/media/font.h>

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
        bool isSystemFont;
        CGFontRef cgFont;
        CTFontDescriptorRef ctFontDesc;

        Font();

        ~Font();

        static Font* createFromSystem(const std::string& name);

        static Font* createFromPath(const std::filesystem::path& path);

        CTFontRef createCTFont(float size, float weight, bool italic) const;

        void destroy();
    };
}
