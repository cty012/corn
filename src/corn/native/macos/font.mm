#import <Foundation/Foundation.h>
#include "macos/font_impl.h"

namespace corn {
    static CGFloat cssWeightToCTFontWeight(float weight) {
        // CSS font weight vs CTFont weight:
        // 100: -1.0
        // 200: -0.8
        // 300: -0.6
        // 400: -0.0
        // 500: 0.23
        // 600: 0.3
        // 700: 0.4
        // 800: 0.56
        // 900: 0.62
        if (weight <= 100) {
            return -1.0f;
        } else if (weight <= 200) {
            return -1.0f + 0.2f * (weight - 100) / 100.0f;
        } else if (weight <= 300) {
            return -0.8f + 0.2f * (weight - 200) / 100.0f;
        } else if (weight <= 400) {
            return -0.6f + 0.6f * (weight - 300) / 100.0f;
        } else if (weight <= 500) {
            return 0.0f + 0.23f * (weight - 400) / 100.0f;
        } else if (weight <= 600) {
            return 0.23f + 0.07f * (weight - 500) / 100.0f;
        } else if (weight <= 700) {
            return 0.3f + 0.10f * (weight - 600) / 100.0f;
        } else if (weight <= 800) {
            return 0.4f + 0.16f * (weight - 700) / 100.0f;
        } else if (weight <= 900) {
            return 0.56f + 0.06f * (weight - 800) / 100.0f;
        } else {
            return 0.62f;  // Cap at 0.62
        }
        return 2.0f;
    }

    Font::Font() : state(FontState::LOADING), isSystemFont(false), cgFont(nullptr), ctFontDesc(nullptr) {}

    Font::~Font() {
        this->destroy();
    }

    void Font::destroy() {
        if (this->cgFont) {
            CFRelease(this->cgFont);
            this->cgFont = nullptr;
        }
        if (this->ctFontDesc) {
            CFRelease(this->ctFontDesc);
            this->ctFontDesc = nullptr;
        }
    }

    Font* Font::createFromSystem(const std::string& name) {
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
        auto* font = static_cast<Font*>(malloc(sizeof(Font)));
        font->isSystemFont = true;
        font->ctFontDesc = ctFontDesc;
        return font;
    }

    Font* Font::createFromPath(const std::filesystem::path& path) {
        NSString* fontPath = [NSString stringWithUTF8String:path.c_str()];
        auto fontURL = (__bridge CFURLRef) [NSURL fileURLWithPath:fontPath];
        CGDataProviderRef dataProvider = CGDataProviderCreateWithURL(fontURL);
        if (!dataProvider) {
            return nullptr;
        }

        CGFontRef cgFont = CGFontCreateWithDataProvider(dataProvider);
        CGDataProviderRelease(dataProvider);
        if (cgFont == nullptr) {
            return nullptr;
        }

        auto* font = static_cast<Font*>(malloc(sizeof(Font)));
        font->isSystemFont = false;
        font->cgFont = cgFont;
        return font;
    }

    CTFontRef Font::createCTFont(float size, float weight, bool italic) const {
        if (this->isSystemFont) {
            // Font weight
            CGFloat ctFontWeight = cssWeightToCTFontWeight(weight);
            CFNumberRef weightNumber = CFNumberCreate(nullptr, kCFNumberCGFloatType, &ctFontWeight);

            // Font italic
            CGFloat isItalic = italic ? 1.0f : 0.0f;
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
            CTFontDescriptorRef styledDesc = CTFontDescriptorCreateCopyWithAttributes(this->ctFontDesc, attrDict);
            CTFontRef styledFont = CTFontCreateWithFontDescriptor(styledDesc, size, nullptr);

            // Release resources
            CFRelease(styledDesc);
            CFRelease(attrDict);
            CFRelease(traitsDict);
            CFRelease(isItalicNumber);
            CFRelease(weightNumber);

            return styledFont;
        } else {
            CTFontSymbolicTraits traits = 0;

            // Weight
            if (weight >= 650) {
                traits |= kCTFontBoldTrait;
            }

            // Italic
            if (italic) {
                traits |= kCTFontItalicTrait;
            }

            // Create the styled font
            CTFontRef baseFont = CTFontCreateWithGraphicsFont(this->cgFont, size, nullptr, nullptr);
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
        }
    }
}
