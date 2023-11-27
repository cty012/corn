/**
 * @file color.h
 * @class Defines color-related data structures and operations.
 */

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

        Color();
        static Color rgb(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255);
        static Color rgb(const RGB& rgbValue);
        static Color rgb(const RGBA& rgbValue);
        [[nodiscard]] RGB getRGB() const;
        [[nodiscard]] RGBA getRGBA() const;
        static Color parse(const std::string& hexString);
        [[nodiscard]] std::string hexString() const;
        [[nodiscard]] std::string hexStringAlpha() const;

        static const Color& RED();
        static const Color& GREEN();
        static const Color& BLUE();
        static const Color& WHITE();
        static const Color& BLACK();

    private:
        Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
        unsigned char r_;
        unsigned char g_;
        unsigned char b_;
        unsigned char a_;
    };
}
