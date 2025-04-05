#include <corn/media/font.h>
#include "macos/font_impl.h"

namespace corn {
    FontManager& FontManager::instance() {
        static FontManager instance;
        return instance;
    }

    FontManager::FontManager() : fonts_(), futures_() {}

    FontManager::~FontManager() {
        this->unloadAll();
    }

    bool FontManager::loadFromSystem(const std::string& name, const std::string& nameInSystem) {
        if (name.empty()) return false;

        // Preload
        this->preloadFromSystem(name, nameInSystem);
        std::lock_guard<std::mutex> lock(this->mutex_);

        // Wait for preload to finish
        {
            std::unique_lock<std::mutex> lockFutures(this->mutexFutures_);
            if (this->futures_.contains(name)) {
                std::future<bool> result = std::move(this->futures_[name]);
                lockFutures.unlock();
                return result.get();
            }
        }

        // Check if load succeed
        std::lock_guard<std::mutex> lockFonts(this->mutexFonts_);
        return this->fonts_.contains(name) && this->fonts_[name]->state == FontState::LOADED;
    }

    bool FontManager::loadFromPath(const std::string& name, const std::filesystem::path& path) {
        if (name.empty()) return false;

        // Preload
        this->preloadFromPath(name, path);
        std::lock_guard<std::mutex> lock(this->mutex_);

        // Wait for preload to finish
        {
            std::unique_lock<std::mutex> lockFutures(this->mutexFutures_);
            if (this->futures_.contains(name)) {
                std::future<bool> result = std::move(this->futures_[name]);
                lockFutures.unlock();
                return result.get();
            }
        }

        // Otherwise check the already loaded fonts
        std::lock_guard<std::mutex> lockFonts(this->mutexFonts_);
        return this->fonts_.contains(name) && this->fonts_[name]->state == FontState::LOADED;
    }

    void FontManager::preloadFromSystem(const std::string& name, const std::string& nameInSystem) {
        if (name.empty()) return;

        std::lock_guard<std::mutex> lock(this->mutex_);
        std::lock_guard<std::mutex> lockFutures(this->mutexFutures_);

        // Check if currently loading
        if (this->futures_.contains(name)) {
            return;
        }

        // Otherwise load from file
        this->futures_[name] = std::async(std::launch::async, [this, &name, &nameInSystem]() {
            {
                std::lock_guard<std::mutex> lockFonts(this->mutexFonts_);
                Font* font = Font::createFromSystem(nameInSystem);
                if (!font) return false;
                font->state = FontState::LOADED;
                this->fonts_[name] = font;
            }
            {
                std::lock_guard<std::mutex> lockFutures(this->mutexFutures_);
                this->futures_.erase(name);
            }
            return true;
        });
    }

    void FontManager::preloadFromPath(const std::string& name, const std::filesystem::path& path) {
        if (name.empty()) return;

        std::lock_guard<std::mutex> lock(this->mutex_);
        std::lock_guard<std::mutex> lockFutures(this->mutexFutures_);

        // Check if currently loading
        if (this->futures_.contains(name)) {
            return;
        }

        // Otherwise load from file
        this->futures_[name] = std::async(std::launch::async, [this, &name, &path]() {
            {
                std::lock_guard<std::mutex> lockFonts(this->mutexFonts_);
                Font* font = Font::createFromPath(path);
                if (!font) return false;
                font->state = FontState::LOADED;
                this->fonts_[name] = font;
            }
            {
                std::lock_guard<std::mutex> lockFutures(this->mutexFutures_);
                this->futures_.erase(name);
            }
            return true;
        });
    }

    bool FontManager::unload(const std::string& name) noexcept {
        std::lock_guard<std::mutex> lock(this->mutex_);
        std::lock_guard<std::mutex> lockFonts(this->mutexFonts_);
        if (!this->fonts_.contains(name)) return false;
        delete this->fonts_[name];
        this->fonts_.erase(name);
        return true;
    }

    size_t FontManager::unloadAll() noexcept {
        for (auto& [name, result] : this->futures_) {
            result.wait();
        }

        std::lock_guard<std::mutex> lock(this->mutex_);
        std::lock_guard<std::mutex> lockFonts(this->mutexFonts_);
        size_t count = this->fonts_.size();
        for (auto& [name, font] : this->fonts_) {
            delete font;
        }
        this->fonts_.clear();
        return count;
    }

    const Font* FontManager::get(const std::string& name) const noexcept {
        std::lock_guard<std::mutex> lock(this->mutex_);
        std::lock_guard<std::mutex> lockFonts(this->mutexFonts_);
        if (!this->fonts_.contains(name) || this->fonts_.at(name)->state != FontState::LOADED) {
            return nullptr;
        }
        return this->fonts_.at(name);
    }

    const Font* FontManager::getDefault() const noexcept {
        std::lock_guard<std::mutex> lock(this->mutex_);

        // If default font is set
        if (!this->defaultFont_.empty()) {
            std::lock_guard<std::mutex> lockFonts(this->mutexFonts_);

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

        // If default font is not set
        std::lock_guard<std::mutex> lockFonts(this->mutexFonts_);

        // Find the first loaded font
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
        std::lock_guard<std::mutex> lock(this->mutex_);
        std::lock_guard<std::mutex> lockFonts(this->mutexFonts_);

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
