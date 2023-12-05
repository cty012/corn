#pragma once

#include <future>
#include <unordered_map>

namespace corn {
    /**
     * @class Font
     * @brief Stores font data.
     */
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

        /**
         * @brief Unload a loaded font.
         * @param name Name of the font to be unloaded.
         * @return Whether the font is successfully unloaded.
         *
         * Unload will fail if the font doesn't exist.
         */
        bool unload(const std::string& name) noexcept;

        /**
         * @param name Name of the font.
         * @return Pointer to the font object if loaded, otherwise null pointer.
         */
        const Font* get(const std::string& name) const noexcept;

    private:
        /// @brief Constructor.
        FontManager();

        /// @brief Destructor.
        ~FontManager();

        FontManager(const FontManager& other) = delete;
        FontManager& operator=(const FontManager& other) = delete;

        /// @brief Stores all fonts.
        std::unordered_map<std::string, Font*> fonts_;

        /// @brief Stores all futures of fonts that are still loading.
        mutable std::unordered_map<std::string, std::future<bool>> futures_;

        /// @brief Mutexes for multithreading.
        mutable std::mutex mutex_, mutexFonts_, mutexFutures_;
    };
}
