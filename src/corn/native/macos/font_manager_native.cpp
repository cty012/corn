#include <corn/media/font_manager.h>
#include "macos/font_family.h"

namespace corn {
    FontManager::FontManager() : fontManagerImpl_(nullptr) {}

    FontManager::~FontManager() {
        this->unloadAll();
    }

    bool FontManager::loadFromSystem(const std::string& name, const std::string& nameInSystem) {
        if (name.empty()) return false;
        this->unload(name);

        // Preload
        FontFamily* fontFamily = FontFamily::createFromSystem(nameInSystem);
        if (!fontFamily) {
            return false;
        }
        fontFamily->state = FontState::LOADED;
        this->fontFamilies_[name] = fontFamily;
        return this->fontFamilies_.contains(name) && this->fontFamilies_[name]->state == FontState::LOADED;
    }

    bool FontManager::loadFromPath(const std::string& name, const std::filesystem::path& path) {
        if (name.empty()) return false;
        this->unload(name);

        // Load the font
        FontFamily* fontFamily = FontFamily::createFromPath(path);
        if (!fontFamily) {
            return false;
        }
        fontFamily->state = FontState::LOADED;
        this->fontFamilies_[name] = fontFamily;
        return true;
    }

    bool FontManager::unload(const std::string& name) noexcept {
        if (!this->fontFamilies_.contains(name)) {
            return false;
        }
        delete this->fontFamilies_[name];
        this->fontFamilies_.erase(name);
        if (this->defaultFont_ == name) {
            this->defaultFont_ = "";
        }
        return true;
    }

    size_t FontManager::unloadAll() noexcept {
        size_t count = this->fontFamilies_.size();
        for (auto& [name, font] : this->fontFamilies_) {
            delete font;
        }
        this->fontFamilies_.clear();
        this->defaultFont_ = "";
        return count;
    }

    const FontFamily* FontManager::get(const std::string& name) const noexcept {
        if (!this->fontFamilies_.contains(name) || this->fontFamilies_.at(name)->state != FontState::LOADED) {
            return nullptr;
        }
        return this->fontFamilies_.at(name);
    }

    const FontFamily* FontManager::getDefault() const noexcept {
        // If default font is set
        if (!this->defaultFont_.empty()) {
            // Check if it is loaded
            if (this->fontFamilies_.contains(this->defaultFont_)) {
                const FontFamily* fontFamily = this->fontFamilies_.at(this->defaultFont_);
                if (fontFamily->state == FontState::LOADED) {
                    return fontFamily;
                }
            }

            // Otherwise remove the default font
            this->defaultFont_ = "";
        }

        // If default font is not set, find the first loaded font
        for (auto& [name, fontFamily] : this->fontFamilies_) {
            if (fontFamily->state == FontState::LOADED) {
                this->defaultFont_ = name;
                return fontFamily;
            }
        }

        // If no font is loaded
        return nullptr;
    }

    bool FontManager::setDefault(const std::string& name) noexcept {
        // Check if font is loaded
        if (this->fontFamilies_.contains(name)) {
            const FontFamily* fontFamily = this->fontFamilies_.at(name);
            if (fontFamily->state == FontState::LOADED) {
                this->defaultFont_ = name;
                return true;
            }
        }

        return false;
    }
}
