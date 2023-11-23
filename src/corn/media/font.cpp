#include <corn/media/font.h>
#include "font_impl.h"

namespace corn {
    Font::Font(): state(FontState::LOADING), sffont() {}

    FontManager& FontManager::instance() {
        static FontManager instance;
        return instance;
    }

    bool FontManager::load(const std::string& name, const std::string& path) {
        // Preload
        this->preload(name, path);
        std::lock_guard<std::mutex> lock(this->mutex);

        // Wait for preload to finish
        {
            std::unique_lock<std::mutex> lockFutures(this->mutexFutures);
            if (this->futures.contains(name)) {
                std::future<bool> result = std::move(this->futures[name]);
                lockFutures.unlock();
                result.wait();
            }
        }

        // Check if load succeed
        std::lock_guard<std::mutex> lockFonts(this->mutexFonts);
        return this->fonts[name]->state == FontState::LOADED;
    }

    void FontManager::preload(const std::string& name, const std::string& path) {
        std::lock_guard<std::mutex> lock(this->mutex);
        std::lock_guard<std::mutex> lockFutures(this->mutexFutures);

        // Check if currently loading
        if (this->futures.contains(name)) {
            return;
        }

        // Otherwise load from file
        this->futures[name] = std::async(std::launch::async, [this, &name, &path]() {
            bool result;
            {
                std::lock_guard<std::mutex> lockFonts(this->mutexFonts);
                Font* font = new Font();
                this->fonts[name] = font;
                result = font->sffont.loadFromFile(path);
                font->state = result ? FontState::LOADED : FontState::LOAD_FAILED;
            }
            {
                std::lock_guard<std::mutex> lockFutures(this->mutexFutures);
                this->futures.erase(name);
            }
            return result;
        });
    }

    bool FontManager::unload(const std::string& name) {
        std::lock_guard<std::mutex> lock(this->mutex);
        std::lock_guard<std::mutex> lockFonts(this->mutexFonts);
        if (!this->fonts.contains(name)) return false;
        delete this->fonts[name];
        this->fonts.erase(name);
        return true;
    }

    const Font* FontManager::get(const std::string& name) const {
        std::lock_guard<std::mutex> lock(this->mutex);
        std::lock_guard<std::mutex> lockFonts(this->mutexFonts);
        if (!this->fonts.contains(name) || this->fonts.at(name)->state != FontState::LOADED) return nullptr;
        return this->fonts.at(name);
    }

    FontManager::FontManager(): fonts(), futures() {}

    FontManager::~FontManager() {
        // Do not free anything
        // Otherwise will have error (exit code 0xC0000409)
        /*
        for (auto& [name, result] : this->futures) {
            result.wait();
        }
        std::lock_guard<std::mutex> lockFonts(this->mutexFonts);
        for (auto& [name, font] : this->fonts) {
            delete font;
        }
         */
    }
}
