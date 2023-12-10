#pragma once

#include <string>
#include <tuple>

namespace corn {
    /**
     * @class Color
     * @brief A class for representing colors.
     */
    class Color {
    public:
        using RGB = std::tuple<unsigned char, unsigned char, unsigned char>;
        using RGBA = std::tuple<unsigned char, unsigned char, unsigned char, unsigned char>;

        /// @brief Useful colors.
        /// @{
        static const Color& WHITE() noexcept;
        static const Color& BLACK() noexcept;
        static const Color& RED() noexcept;
        static const Color& GREEN() noexcept;
        static const Color& BLUE() noexcept;
        /// @}

        /// @brief Default constructor.
        Color() noexcept;

        /// @brief Construct color from R, G, B (and A) values.
        static Color rgb(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255) noexcept;

        /// @brief Construct from a 3-tuple representing RGB value.
        static Color rgb(const RGB& rgbValue) noexcept;

        /// @brief Construct from a 4-tuple representing RGBA value.
        static Color rgb(const RGBA& rgbValue) noexcept;

        /// @brief Get the RGB value as a tuple.
        [[nodiscard]] RGB getRGB() const noexcept;

        /// @brief Get the RGBA value as a tuple.
        [[nodiscard]] RGBA getRGBA() const noexcept;

        /**
         * @brief Parse the color from a RGB(A) hex string.
         * @param hexString The hex string to parse.
         *
         * The hex string should begin with an optional hashtag `#`. Then it is followed by 6 hex digits (0-9 or a-f,
         * case-insensitive) representing the RGB values. An optional two hex digits represents the alpha value.
         */
        static Color parse(const std::string& hexString);

        /**
         * @return The hex-representation of the color without the alpha value.
         *
         * The string will start with a hashtag `#` and 6 hex digits (0-9 or a-f) representing the RGB values.
         */
        [[nodiscard]] std::string hexString() const noexcept;

        /**
         * @return The hex-representation of the color with the alpha value.
         *
         * The string will start with a hashtag `#` and 8 hex digits (0-9 or a-f) representing the RGBA values.
         */
        [[nodiscard]] std::string hexStringAlpha() const noexcept;

    private:
        /// @brief Private constructor.
        Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a) noexcept;

        /// @brief The RGBA values.
        unsigned char r_, g_, b_, a_;
    };
}
