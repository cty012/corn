#pragma once

#include <filesystem>
#include <string>
#include <CoreText/CoreText.h>
#include "../../../media/font_state.h"

namespace corn {
    class FontFamily {
    public:
        FontState state;
        bool isSystemFont;
        CGFontRef cgFont;
        CTFontDescriptorRef ctFontDesc;

        FontFamily();

        ~FontFamily();

        void destroy();

        static FontFamily* createFromSystem(const std::string& name);

        static FontFamily* createFromPath(const std::filesystem::path& path);

        CTFontRef createCTFont(float size, float weight, bool italic) const;
    };
}
