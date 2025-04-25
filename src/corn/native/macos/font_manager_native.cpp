#include <ranges>
#include <corn/media/font_manager.h>
#include "macos/font_family.h"

namespace corn {
    FontManager::FontManager() : fontManagerImpl_(nullptr) {}

    FontManager::~FontManager() {
        this->unloadAllFontFamilies();
    }

    bool FontManager::loadFontFamilyFromSystem(const std::string& name, const std::string& nameInSystem) {
        if (name.empty()) return false;
        this->unloadFontFamily(name);

        // Create the font family
        FontFamily* fontFamily = FontFamily::createFromSystem(nameInSystem);
        if (!fontFamily) {
            return false;
        }

        // Add to the font manager
        this->fontFamilies_[name] = fontFamily;
        return true;
    }

    bool FontManager::loadFontFamilyFromPath(const std::string& name, const std::filesystem::path& path) {
        if (name.empty()) return false;
        this->unloadFontFamily(name);

        // Create the font family
        FontFamily* fontFamily = FontFamily::createFromPath(path);
        if (!fontFamily) {
            return false;
        }

        // Add to the font manager
        this->fontFamilies_[name] = fontFamily;
        return true;
    }

    bool FontManager::loadFontFaceFromPath(const std::string& name, const std::filesystem::path& path, const FontVariant& fontVariant) {
        if (!this->fontFamilies_.contains(name)) {
            return false;
        }
        FontFamily* fontFamily = this->fontFamilies_[name];

        // Add the font face
        return fontFamily->addFontFace(path, fontVariant);
    }

    bool FontManager::unloadFontFamily(const std::string& name) {
        if (!this->fontFamilies_.contains(name)) {
            return false;
        }

        // Remove the font family
        delete this->fontFamilies_[name];
        this->fontFamilies_.erase(name);
        if (this->defaultFont_ == name) {
            this->defaultFont_.clear();
        }
        return true;
    }

    bool FontManager::unloadFontFace(const std::string& name, const FontVariant& fontVariant) {
        if (!this->fontFamilies_.contains(name) || fontVariant == FontVariant()) {
            return false;
        }

        // Remove the font face
        return this->fontFamilies_[name]->removeFontFace(fontVariant);
    }

    size_t FontManager::unloadAllFontFamilies() {
        size_t count = this->fontFamilies_.size();
        for (FontFamily* fontFamily : this->fontFamilies_ | std::views::values) {
            delete fontFamily;
        }
        this->fontFamilies_.clear();
        this->defaultFont_.clear();
        return count;
    }

    const FontFamily* FontManager::get(const std::string& name) const noexcept {
        auto it = this->fontFamilies_.find(name);
        if (it != this->fontFamilies_.end()) {
            return it->second;
        }
        return nullptr;
    }

    const FontFamily* FontManager::getDefault() const noexcept {
        // If the default font is set
        if (!this->defaultFont_.empty()) {
            // Check if it is loaded
            auto it = this->fontFamilies_.find(this->defaultFont_);
            if (it != this->fontFamilies_.end()) {
                return it->second;
            }

            // Otherwise, remove the default font
            this->defaultFont_.clear();
        }

        // If the default font is not set, find the first loaded font
        if (!this->fontFamilies_.empty()) {
            return this->fontFamilies_.begin()->second;
        }

        // If no font is loaded
        return nullptr;
    }

    bool FontManager::setDefault(const std::string& name) noexcept {
        // Check if the font is loaded
        if (this->fontFamilies_.contains(name)) {
            this->defaultFont_ = name;
            return true;
        }

        return false;
    }
}
