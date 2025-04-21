#pragma once

// This is because of a bug in CMake.
// Failing to include this before all Windows headers will result in the symbol "==" being exported into exports.def.
#include <guiddef.h>

#include <filesystem>
#include <string>
#include <unordered_map>
#include <dwrite.h>
#include <corn/media/font_manager.h>

namespace corn {
    class FontFace {
    public:
        IDWriteFontFile* fontFile;
        IDWriteFontCollection* fontCollection;
        std::wstring name;

        FontFace();
        FontFace(const std::filesystem::path& path, IDWriteFontCollectionLoader* fontCollectionLoader);
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
        /// @brief Only used for system fonts. Otherwise, is empty string.
        std::wstring fontFamilyName;
        std::unordered_map<FontVariant, FontFace> fontFaces;

        FontFamily();
        ~FontFamily();

        void destroy();

        static FontFamily* createFromSystem(const std::string& name);
        static FontFamily* createFromPath(
                const std::filesystem::path& path,
                IDWriteFontCollectionLoader* fontCollectionLoader);

        bool addFontFace(
                const std::filesystem::path& path, const FontVariant& fontVariant,
                IDWriteFontCollectionLoader* fontCollectionLoader);
        bool removeFontFace(const FontVariant& fontVariant);
    };
}
