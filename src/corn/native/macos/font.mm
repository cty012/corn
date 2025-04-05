#import <Foundation/Foundation.h>
#include "macos/font_impl.h"

namespace corn {
    Font::Font() : state(FontState::LOADING), cgFont(nullptr) {}

    Font::~Font() {
        this->destroy();
    }

    void Font::destroy() {
        if (this->cgFont) {
            CFRelease(this->cgFont);
            this->cgFont = nullptr;
        }
    }

    Font* Font::createFromSystem(const std::string& name) {
        NSString* fontName = [NSString stringWithUTF8String:name.c_str()];
        CGFontRef cgFont = CGFontCreateWithFontName((CFStringRef) fontName);
        if (!cgFont) {
            return nullptr;
        }

        auto* font = static_cast<Font*>(malloc(sizeof(Font)));
        font->cgFont = cgFont;
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
        font->cgFont = cgFont;
        return font;
    }
}
