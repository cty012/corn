#pragma once

#include <future>
#include <unordered_map>

namespace corn {
    class Font;

    /**
     * @class FontVariant
     * @brief Variants of the font, including weight, slant, and other styles.
     *
     * @todo More styles (allow loading custom style fonts)
     * 1. Light, Regular, Semi-bold, Bold, Heavy
     * 2. Regular, Italic
     * 3. Regular, Underline
     *
     * @see FontManager
     */
    enum class FontVariant {
        REGULAR, BOLD, ITALIC, UNDERLINE,
    };

    /**
     * @class FontManager
     * @brief Manages font loading.
     *
     * @see FontVariant
     */
    class FontManager {
    public:
        static FontManager& instance();

        /**
         * @brief Loads a font file into the font manager.
         * @param name Name of the font in the font manager.
         * @param path Path to the font file.
         * @return Whether the font is successfully loaded.
         */
        bool load(const std::string& name, const std::string& path);

        /**
         * @brief Loads a font file asynchronously into the font manager.
         * @param name Name of the font in the font manager.
         * @param path Path to the font file.
         */
        void preload(const std::string& name, const std::string& path);
        bool unload(const std::string& name);
        const Font* get(const std::string& name) const;

    private:
        FontManager();
        ~FontManager();
        FontManager(const FontManager& other) = delete;
        FontManager& operator=(const FontManager& other) = delete;

        std::unordered_map<std::string, Font*> fonts_;
        mutable std::unordered_map<std::string, std::future<bool>> futures_;
        mutable std::mutex mutex_;
        mutable std::mutex mutexFonts_;
        mutable std::mutex mutexFutures_;
    };
}
