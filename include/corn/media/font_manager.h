#pragma once

#include <filesystem>
#include <string>
#include <unordered_map>

namespace corn {
    /**
     * @class FontFamily
     * @brief Stores font family data.
     *
     * Declaration is private.
     */
    class FontFamily;

    constexpr float FONT_WEIGHT_THIN = 100.0f;
    constexpr float FONT_WEIGHT_EXTRA_LIGHT = 200.0f;
    constexpr float FONT_WEIGHT_LIGHT = 300.0f;
    constexpr float FONT_WEIGHT_REGULAR = 400.0f;
    constexpr float FONT_WEIGHT_MEDIUM = 500.0f;
    constexpr float FONT_WEIGHT_SEMI_BOLD = 600.0f;
    constexpr float FONT_WEIGHT_BOLD = 700.0f;
    constexpr float FONT_WEIGHT_EXTRA_BOLD = 800.0f;
    constexpr float FONT_WEIGHT_BLACK = 900.0f;

    /**
     * @class FontVariant
     * @brief Variants of the font family, including weight and italic
     *
     * weight: 100 to 900 (default: 400)
     * italic: false or true (default: false)
     *
     * @see FontManager
     */
    struct FontVariant {
        float weight = FONT_WEIGHT_REGULAR;
        bool italic = false;

        friend bool operator==(const FontVariant& lhs, const FontVariant& rhs) noexcept = default;
    };

    enum class FontPosition {
        REGULAR, SUBSCRIPT, SUPERSCRIPT,
    };

    /**
     * @class FontManager
     * @brief Manages font loading.
     */
    class FontManager {
    public:
        static FontManager& instance();

        /**
         * @brief Loads a font file into the font manager.
         * @param name Name of the font family in the font manager. Must not be empty.
         * @param nameInSystem Name of the font family in the system.
         * @return Whether the font is successfully loaded.
         */
        bool loadFontFamilyFromSystem(const std::string& name, const std::string& nameInSystem);

        /**
         * @brief Loads a font file into the font manager.
         * @param name Name of the font family in the font manager. Must not be empty.
         * @param path Path to the font file.
         * @return Whether the font is successfully loaded.
         *
         * The font face stored in the font file will be used as the default font face of the font family.
         */
        bool loadFontFamilyFromPath(const std::string& name, const std::filesystem::path& path);

        /**
         * @brief Loads a font face as the variant of the font family.
         * @param name Name of the font family in the font manager. Must already be loaded.
         * @param path Path to the variant font file.
         * @param variant Font variant.
         * @return Whether the font is successfully loaded.
         *
         * The font face stored in the font file will be used as the variant font face of the font family.
         */
        bool loadFontFaceFromPath(const std::string& name, const std::filesystem::path& path, const FontVariant& variant);

        /**
         * @brief Unload a loaded font family.
         * @param name Name of the font family to be unloaded.
         * @return Whether the font family is successfully unloaded.
         *
         * Unload will fail if the font family doesn't exist.
         */
        bool unloadFontFamily(const std::string& name);

        /**
         * @brief Unload a loaded font face.
         * @param name Name of the font family.
         * @param variant Variant of the font family.
         * @return Whether the font face is successfully unloaded.
         *
         * Unload will fail if the specified variant of the font family doesn't exist. Also fails if the font face is
         * the default font face of the font family.
         */
        bool unloadFontFace(const std::string& name, const FontVariant& variant);

        /**
         * @brief Unload all loaded font families.
         * @return Number of font families successfully unloaded.
         */
        size_t unloadAllFontFamilies();

        /**
         * @param name Name of the font family.
         * @return Pointer to the font family if loaded, otherwise null pointer.
         */
        [[nodiscard]] const FontFamily* get(const std::string& name) const noexcept;

        /// @return Pointer to the default font family.
        [[nodiscard]] const FontFamily* getDefault() const noexcept;

        /**
         * @brief Set the default font family.
         * @param name Name of the font family.
         * @return Whether the font family is successfully set as default.
         */
        bool setDefault(const std::string& name) noexcept;

    private:
        /// @brief Constructor.
        FontManager();

        /// @brief Destructor.
        ~FontManager();

        FontManager(const FontManager& other) = delete;
        FontManager& operator=(const FontManager& other) = delete;

        /// @brief Stores all font families.
        std::unordered_map<std::string, FontFamily*> fontFamilies_;

        /// @brief The name of the default font family.
        mutable std::string defaultFont_;

        /// @brief Pimpl idiom.
        class FontManagerImpl;
        FontManagerImpl* fontManagerImpl_;
    };
}

namespace std {
    template <>
    struct hash<corn::FontVariant> {
        size_t operator()(const corn::FontVariant& fv) const noexcept {
            size_t h1 = std::hash<float>{}(fv.weight);
            size_t h2 = std::hash<bool>{}(fv.italic);
            return h1 ^ (h2 << 1);  // combine hashes
        }
    };
}
