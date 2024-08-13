#pragma once

#include <string>
#include <tuple>
#include <corn/geometry/deg.h>

namespace corn {
    /**
     * @class Color
     * @brief A class for representing colors.
     */
    class Color {
    public:
        using RGB = std::tuple<unsigned char, unsigned char, unsigned char>;
        using RGBA = std::tuple<unsigned char, unsigned char, unsigned char, unsigned char>;
        using HSL = std::tuple<Deg, float, float>;
        using HSLA = std::tuple<Deg, float, float, unsigned char>;

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

        /// @brief Construct from a 3-tuple representing RGB values.
        static Color rgb(const RGB& rgbValues) noexcept;

        /// @brief Construct from a 4-tuple representing RGBA values.
        static Color rgb(const RGBA& rgbaValues) noexcept;

        /// @brief Construct from a 3-tuple representing HSL values.
        static Color hsl(const HSL& hslValues) noexcept;

        /// @brief Construct from a 4-tuple representing HSLA values.
        static Color hsl(const HSLA& hslaValues) noexcept;

        /// @brief Get the RGB values as a tuple.
        [[nodiscard]] RGB getRGB() const noexcept;

        /// @brief Get the RGBA values as a tuple.
        [[nodiscard]] RGBA getRGBA() const noexcept;

        /// @brief Get the HSL values as a tuple.
        [[nodiscard]] HSL getHSL() const noexcept;

        /// @brief Get the HSLA values as a tuple.
        [[nodiscard]] HSLA getHSLA() const noexcept;

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
