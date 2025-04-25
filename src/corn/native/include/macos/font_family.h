#pragma once

#include <filesystem>
#include <string>
#include <CoreText/CoreText.h>
#include <corn/media/font_manager.h>

namespace corn {
    class FontFace {
    public:
        CGFontRef cgFont;

        FontFace();
        explicit FontFace(const std::filesystem::path& path);
        ~FontFace();
        FontFace(const FontFace&) = delete;
        FontFace& operator=(const FontFace&) = delete;
        FontFace(FontFace&&) noexcept;
        FontFace& operator=(FontFace&&) noexcept;

        void destroy();

        [[nodiscard]] bool isValid() const;
    };

    class FontFamily {
    public:
        CTFontDescriptorRef systemFontDescriptor;
        std::unordered_map<FontVariant, FontFace> fontFaces;

        FontFamily();
        ~FontFamily();

        void destroy();

        static FontFamily* createFromSystem(const std::string& name);
        static FontFamily* createFromPath(const std::filesystem::path& path);

        bool addFontFace(const std::filesystem::path& path, const FontVariant& fontVariant);
        bool removeFontFace(const FontVariant& fontVariant);

        [[nodiscard]] CTFontRef createCTFont(float size, const FontVariant& fontVariant) const;

    private:
        // Assume that the system font descriptor is valid
        [[nodiscard]] CTFontRef createCTFontWithSystem(float size, const FontVariant& fontVariant) const;

        // Assume that the variant exists
        [[nodiscard]] CTFontRef createCTFontWithVariant(float size, const FontVariant& fontVariant) const;

        // The base font must exist
        [[nodiscard]] CTFontRef createCTFontWithBase(float size, const FontVariant& fontVariant) const;
    };
}
