#import <Foundation/Foundation.h>
#include "macos/font_family.h"
#include "macos/utils.h"

namespace corn {
    FontFace::FontFace() : cgFont(nullptr) {}

    FontFace::FontFace(const std::filesystem::path& path) {
        NSString* fontPath = [NSString stringWithUTF8String:path.c_str()];
        auto fontURL = (__bridge CFURLRef) [NSURL fileURLWithPath:fontPath];
        CGDataProviderRef dataProvider = CGDataProviderCreateWithURL(fontURL);
        if (!dataProvider) {
            return;
        }

        this->cgFont = CGFontCreateWithDataProvider(dataProvider);
        CGDataProviderRelease(dataProvider);
    }

    FontFace::~FontFace() {
        this->destroy();
    }

    FontFace::FontFace(FontFace&& other) noexcept {
        cgFont = other.cgFont;
        other.cgFont = nullptr;
    }

    FontFace& FontFace::operator=(FontFace&& other) noexcept {
        if (this == &other) return *this;
        this->destroy();
        cgFont = other.cgFont;
        other.cgFont = nullptr;
        return *this;
    }

    void FontFace::destroy() {
        if (cgFont) {
            CGFontRelease(cgFont);
            cgFont = nullptr;
        }
    }

    bool FontFace::isValid() const {
        return cgFont != nullptr;
    }

    FontFamily::FontFamily() : systemFontDescriptor(nullptr) {}

    FontFamily::~FontFamily() {
        this->destroy();
    }

    void FontFamily::destroy() {
        if (this->systemFontDescriptor) {
            CFRelease(this->systemFontDescriptor);
            this->systemFontDescriptor = nullptr;
        }
        this->fontFaces.clear();
    }

    FontFamily* FontFamily::createFromSystem(const std::string& name) {
        CFStringRef fontName = CFStringCreateWithCString(kCFAllocatorDefault, name.c_str(), kCFStringEncodingUTF8);
        CTFontDescriptorRef ctFontDesc = CTFontDescriptorCreateWithNameAndSize(fontName, 12.0f);

        // Check if the font descriptor is valid
        CTFontRef matchedFont = CTFontCreateWithFontDescriptor(ctFontDesc, 0.0, nullptr);
        if (!matchedFont) {
            // Invalid font
            CFRelease(ctFontDesc);
            CFRelease(fontName);
            return nullptr;
        }
        CFRelease(matchedFont);
        CFRelease(fontName);

        // Create the font object
        auto* font = new FontFamily();
        font->systemFontDescriptor = ctFontDesc;
        return font;
    }

    FontFamily* FontFamily::createFromPath(const std::filesystem::path& path) {
        FontFace fontFace(path);
        if (!fontFace.isValid()) {
            return nullptr;
        }
        auto* font = new FontFamily();
        font->fontFaces[FontVariant()] = std::move(fontFace);
        return font;
    }

    bool FontFamily::addFontFace(const std::filesystem::path& path, const FontVariant& fontVariant) {
        FontFace fontFace(path);
        if (!fontFace.isValid()) {
            return false;
        }
        this->fontFaces[fontVariant] = std::move(fontFace);
        return true;
    }

    bool FontFamily::removeFontFace(const corn::FontVariant& fontVariant) {
        return this->fontFaces.erase(fontVariant);
    }

    CTFontRef FontFamily::createCTFont(float size, const FontVariant& fontVariant) const {
        // First check if the font variant exists
        if (this->fontFaces.contains(fontVariant)) {
            // Create the font with the specified variant
            CTFontRef styledFont = this->createCTFontWithVariant(size, fontVariant);
            if (styledFont) {
                return styledFont;
            }
        }

        // Otherwise, Use the system font or synthesize with the base font
        if (this->systemFontDescriptor) {
            return this->createCTFontWithSystem(size, fontVariant);
        } else {
            return this->createCTFontWithBase(size, FontVariant());
        }
    }

    CTFontRef FontFamily::createCTFontWithSystem(float size, const corn::FontVariant& fontVariant) const {
        // Font weight
        CGFloat ctFontWeight = cssWeightToCTFontWeight(fontVariant.weight);
        CFNumberRef weightNumber = CFNumberCreate(nullptr, kCFNumberCGFloatType, &ctFontWeight);

        // Font italic
        CGFloat isItalic = fontVariant.italic ? 1.0f : 0.0f;
        CFNumberRef isItalicNumber = CFNumberCreate(nullptr, kCFNumberCGFloatType, &isItalic);

        // Set the font family and size
        CFDictionaryRef traitsDict = CFDictionaryCreate(
                nullptr,
                (const void*[]) { kCTFontWeightTrait, kCTFontSlantTrait, },
                (const void*[]) { weightNumber, isItalicNumber, },
                2, nullptr, nullptr);
        CFDictionaryRef attrDict = CFDictionaryCreate(
                nullptr,
                (const void*[]) { kCTFontTraitsAttribute },
                (const void*[]) { traitsDict },
                1, nullptr, nullptr);

        // Create the styled font descriptor
        CTFontDescriptorRef styledDesc = CTFontDescriptorCreateCopyWithAttributes(this->systemFontDescriptor, attrDict);
        CTFontRef styledFont = CTFontCreateWithFontDescriptor(styledDesc, size, nullptr);

        // Release resources
        CFRelease(styledDesc);
        CFRelease(attrDict);
        CFRelease(traitsDict);
        CFRelease(isItalicNumber);
        CFRelease(weightNumber);

        return styledFont;
    }

    CTFontRef FontFamily::createCTFontWithVariant(float size, const corn::FontVariant& fontVariant) const {
        const FontFace& fontFace = this->fontFaces.at(fontVariant);
        return CTFontCreateWithGraphicsFont(fontFace.cgFont, size, nullptr, nullptr);
    }

    CTFontRef FontFamily::createCTFontWithBase(float size, const FontVariant& fontVariant) const {
        CTFontSymbolicTraits traits = 0;

        // Weight
        if (fontVariant.weight >= 650) {
            traits |= kCTFontBoldTrait;
        }

        // Italic
        if (fontVariant.italic) {
            traits |= kCTFontItalicTrait;
        }

        // Create the styled font
        const FontFace& fontFace = this->fontFaces.at(FontVariant());
        CTFontRef baseFont = CTFontCreateWithGraphicsFont(fontFace.cgFont, size, nullptr, nullptr);
        CTFontRef styledFont = CTFontCreateCopyWithSymbolicTraits(
                baseFont, size, nullptr, traits, kCTFontBoldTrait | kCTFontItalicTrait);

        if (styledFont) {
            // Release resources
            CFRelease(baseFont);
            return styledFont;
        } else {
            // Fallback to the base font if styled font creation fails
            return baseFont;
        }

        return styledFont;
    }
}
