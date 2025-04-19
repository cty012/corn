#include <ranges>
#include "win/font_family.h"
#include "win/utils.h"

namespace corn {
    /// FontFace

    FontFace::FontFace(): fontFile(nullptr), fontCollection(nullptr) {}

    FontFace::FontFace(const std::filesystem::path& path, IDWriteFontCollectionLoader* fontCollectionLoader) {
        IDWriteFactory5* factory = getDWriteFactory5();

        // Create a new Font file
        this->fontFile = nullptr;
        HRESULT hr = factory->CreateFontFileReference(
            utf8To16(path.string()).c_str(),
            nullptr,
            &this->fontFile
        );
        if (FAILED(hr) || !this->fontFile) {
            this->fontFile = nullptr;
            return;
        }

        // Get the family name
        hr = getFontFileFamilyName(this->fontFile, &this->name);
        if (FAILED(hr) || this->name.empty()) {
            this->fontFile->Release();
            this->fontFile = nullptr;
            return;
        }

        // Create a new Font collection
        hr = factory->CreateCustomFontCollection(
            fontCollectionLoader,
            &this->fontFile, sizeof(this->fontFile),
            &this->fontCollection
        );
        if (FAILED(hr) || !this->fontCollection) {
            this->fontFile->Release();
            this->fontFile = nullptr;
            this->fontCollection = nullptr;
            this->name.clear();
            return;
        }
    }

    FontFace::~FontFace() {
        this->destroy();
    }

    FontFace::FontFace(FontFace&& other) noexcept : fontFile(other.fontFile), fontCollection(other.fontCollection) {
        other.fontFile = nullptr;
        this->fontCollection = other.fontCollection;
    }

    FontFace& FontFace::operator=(FontFace&& other) noexcept {
        if (this == &other) return *this;

        this->fontFile = other.fontFile;
        this->fontCollection = other.fontCollection;
        other.fontFile = nullptr;
        other.fontCollection = nullptr;

        return *this;
    }

    void FontFace::destroy() {
        if (this->fontFile) {
            this->fontFile->Release();
            this->fontFile = nullptr;
        }
        if (this->fontCollection) {
            this->fontCollection->Release();
            this->fontCollection = nullptr;
        }
        this->name.clear();
    }

    bool FontFace::isValid() const {
        return this->fontFile != nullptr && this->fontCollection != nullptr && !this->name.empty();
    }

    /// FontFamily

    FontFamily::FontFamily() = default;

    FontFamily::~FontFamily() {
        this->destroy();
    }

    void FontFamily::destroy() {
        this->fontFaces.clear();
    }

    FontFamily* FontFamily::createFromSystem(const std::string& name) {
        IDWriteFactory5* factory = getDWriteFactory5();

        // Create a new Font collection
        IDWriteFontCollection* fontCollection;
        HRESULT hr = factory->GetSystemFontCollection(&fontCollection);
        if (FAILED(hr) || !fontCollection) {
            return nullptr;
        }

        // Check the font family name exists
        UINT index;
        BOOL exists;
        std::wstring familyName = utf8To16(name);
        hr = fontCollection->FindFamilyName(familyName.c_str(), &index, &exists);
        if (FAILED(hr) || !exists) {
            fontCollection->Release();
            return nullptr;
        }

        // Create a new Font family
        auto* fontFamily = new FontFamily();
        fontFamily->fontFamilyName = familyName;
        fontCollection->Release();
        return fontFamily;
    }

    FontFamily* FontFamily::createFromPath(
            const std::filesystem::path& path,
            IDWriteFontCollectionLoader* fontCollectionLoader) {

        FontFace fontFace(path, fontCollectionLoader);
        if (!fontFace.isValid()) {
            return nullptr;
        }
        auto* fontFamily = new FontFamily();
        fontFamily->fontFaces[FontVariant()] = std::move(fontFace);
        return fontFamily;
    }

    bool FontFamily::addFontFace(
            const std::filesystem::path& path, const FontVariant& fontVariant,
            IDWriteFontCollectionLoader* fontCollectionLoader) {

        FontFace fontFace(path, fontCollectionLoader);
        if (!fontFace.isValid()) {
            return false;
        }
        this->fontFaces[fontVariant] = std::move(fontFace);
        return true;
    }

    bool FontFamily::removeFontFace(const FontVariant& fontVariant) {
        return this->fontFaces.erase(fontVariant);
    }
}
