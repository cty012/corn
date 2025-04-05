#pragma once

#include <filesystem>
#include <future>
#include <string>
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
    enum class FontWeight {
        LIGHT, REGULAR, SEMI_BOLD, BOLD, HEAVY,
    };

    enum class FontSlant {
        REGULAR, ITALIC,
    };

    enum class FontDecoration {
        REGULAR, UNDERLINE,
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
         * @param name Name of the font in the font manager. Must not be empty.
         * @param nameInSystem Name of the font in the system.
         * @return Whether the font is successfully loaded.
         */
        bool loadFromSystem(const std::string& name, const std::string& nameInSystem);

        /**
         * @brief Loads a font file into the font manager.
         * @param name Name of the font in the font manager. Must not be empty.
         * @param path Path to the font file.
         * @return Whether the font is successfully loaded.
         */
        bool loadFromPath(const std::string& name, const std::filesystem::path& path);

        /**
         * @brief Loads a font file asynchronously into the font manager.
         * @param name Name of the font in the font manager. Must not be empty.
         * @param nameInSystem Name of the font in the system.
         */
        void preloadFromSystem(const std::string& name, const std::string& nameInSystem);

        /**
         * @brief Loads a font file asynchronously into the font manager.
         * @param name Name of the font in the font manager. Must not be empty.
         * @param path Path to the font file.
         */
        void preloadFromPath(const std::string& name, const std::filesystem::path& path);

        /**
         * @brief Unload a loaded font.
         * @param name Name of the font to be unloaded.
         * @return Whether the font is successfully unloaded.
         *
         * Unload will fail if the font doesn't exist.
         */
        bool unload(const std::string& name) noexcept;

        /**
         * @brief Unload all loaded fonts.
         * @return Number of fonts successfully unloaded.
         */
        size_t unloadAll() noexcept;

        /**
         * @param name Name of the font.
         * @return Pointer to the font object if loaded, otherwise null pointer.
         */
        [[nodiscard]] const Font* get(const std::string& name) const noexcept;

        /// @return Pointer to the default font object.
        [[nodiscard]] const Font* getDefault() const noexcept;

        /**
         * @brief Set the default font.
         * @param name Name of the font.
         * @return Whether the font is successfully set as default.
         */
        bool setDefault(const std::string& name) noexcept;

    private:
        /// @brief Constructor.
        FontManager();

        /// @brief Destructor.
        ~FontManager();

        FontManager(const FontManager& other) = delete;
        FontManager& operator=(const FontManager& other) = delete;

        /// @brief Stores all fonts.
        std::unordered_map<std::string, Font*> fonts_;

        /// @brief The name of the default font.
        mutable std::string defaultFont_;

        /// @brief Stores all futures of fonts that are still loading.
        mutable std::unordered_map<std::string, std::future<bool>> futures_;

        /// @brief Mutexes for multithreading.
        mutable std::mutex mutex_;  // General purpose
        mutable std::mutex mutexFonts_;  // For fonts_
        mutable std::mutex mutexFutures_;  // For futures_
    };
}
