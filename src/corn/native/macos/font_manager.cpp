#include <corn/media/font.h>
#include "macos/font_impl.h"

namespace corn {
    FontManager& FontManager::instance() {
        static FontManager instance;
        return instance;
    }

    FontManager::FontManager() = default;

    FontManager::~FontManager() {
        this->unloadAll();
    }

    bool FontManager::loadFromSystem(const std::string& name, const std::string& nameInSystem) {
        if (name.empty()) return false;
        this->unload(name);

        // Preload
        Font* font = Font::createFromSystem(nameInSystem);
        if (!font) {
            return false;
        }
        font->state = FontState::LOADED;
        this->fonts_[name] = font;
        return this->fonts_.contains(name) && this->fonts_[name]->state == FontState::LOADED;
    }

    bool FontManager::loadFromPath(const std::string& name, const std::filesystem::path& path) {
        if (name.empty()) return false;
        this->unload(name);

        // Load the font
        Font* font = Font::createFromPath(path);
        if (!font) {
            return false;
        }
        font->state = FontState::LOADED;
        this->fonts_[name] = font;
        return true;
    }

    bool FontManager::unload(const std::string& name) noexcept {
        if (!this->fonts_.contains(name)) {
            return false;
        }
        delete this->fonts_[name];
        this->fonts_.erase(name);
        if (this->defaultFont_ == name) {
            this->defaultFont_ = "";
        }
        return true;
    }

    size_t FontManager::unloadAll() noexcept {
        size_t count = this->fonts_.size();
        for (auto& [name, font] : this->fonts_) {
            delete font;
        }
        this->fonts_.clear();
        this->defaultFont_ = "";
        return count;
    }

    const Font* FontManager::get(const std::string& name) const noexcept {
        if (!this->fonts_.contains(name) || this->fonts_.at(name)->state != FontState::LOADED) {
            return nullptr;
        }
        return this->fonts_.at(name);
    }

    const Font* FontManager::getDefault() const noexcept {
        // If default font is set
        if (!this->defaultFont_.empty()) {
            // Check if it is loaded
            if (this->fonts_.contains(this->defaultFont_)) {
                const Font* target = this->fonts_.at(this->defaultFont_);
                if (target->state == FontState::LOADED) {
                    return target;
                }
            }

            // Otherwise remove the default font
            this->defaultFont_ = "";
        }

        // If default font is not set, find the first loaded font
        for (auto& [name, font] : this->fonts_) {
            if (font->state == FontState::LOADED) {
                this->defaultFont_ = name;
                return font;
            }
        }

        // If no font is loaded
        return nullptr;
    }

    bool FontManager::setDefault(const std::string& name) noexcept {
        // Check if font is loaded
        if (this->fonts_.contains(name)) {
            const Font* font = this->fonts_.at(name);
            if (font->state == FontState::LOADED) {
                this->defaultFont_ = name;
                return true;
            }
        }

        return false;
    }
}
