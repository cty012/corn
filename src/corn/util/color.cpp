#include <cstdio>
#include <corn/util/color.h>

namespace corn {
    Color::Color(): red(), green(), blue(), alpha() {}

    Color::Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
        : red(r), green(g), blue(b), alpha(a) {}

    Color Color::rgb(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
        return {r, g, b, a};
    }

    Color Color::rgb(const Color::RGB& rgbValue) {
        auto [r, g, b] = rgbValue;
        return {r, g, b, 255};
    }

    Color Color::rgb(const Color::RGBA &rgbValue) {
        auto [r, g, b, a] = rgbValue;
        return {r, g, b, a};
    }

    Color::RGB Color::getRGB() const {
        return std::make_tuple(this->red, this->green, this->blue);
    }

    Color::RGBA Color::getRGBA() const {
        return std::make_tuple(this->red, this->green, this->blue, this->alpha);
    }

    std::string Color::hexString() const {
        char hexStr[8];
        std::snprintf(hexStr, sizeof(hexStr), "#%02X%02X%02X", this->red, this->green, this->blue);
        return {hexStr};
    }
}
