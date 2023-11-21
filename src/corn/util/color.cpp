#include <cstdio>
#include <regex>
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

    Color Color::parse(const std::string& hexString) {
        static const std::regex color_regex(R"(#?([a-fA-F0-9]{6})([a-fA-F0-9]{2})?)");
        std::smatch color_match;

        if (!std::regex_match(hexString, color_match, color_regex)) {
            throw std::invalid_argument("Invalid hex color format");
        }

        const std::string& rgbString = color_match[1].str();
        int hasAlpha = color_match[2].matched;
        unsigned char r = static_cast<unsigned char>(std::stoi(rgbString.substr(0, 2), nullptr, 16));
        unsigned char g = static_cast<unsigned char>(std::stoi(rgbString.substr(2, 2), nullptr, 16));
        unsigned char b = static_cast<unsigned char>(std::stoi(rgbString.substr(4, 2), nullptr, 16));
        unsigned char a = hasAlpha ? static_cast<unsigned char>(std::stoi(color_match[2], nullptr, 16)) : 0xff;
        return Color::rgb(r, g, b, a);
    }

    std::string Color::hexString() const {
        char hexStr[8];
        std::snprintf(hexStr, sizeof(hexStr), "#%02X%02X%02X", this->red, this->green, this->blue);
        return {hexStr};
    }

    std::string Color::hexStringAlpha() const {
        char hexStr[10];
        std::snprintf(hexStr, sizeof(hexStr), "#%02X%02X%02X%02X", this->red, this->green, this->blue, this->alpha);
        return {hexStr};
    }

    const Color& Color::RED() {
        static const Color color = Color::rgb(255, 0, 0);
        return color;
    }

    const Color& Color::GREEN() {
        static const Color color = Color::rgb(0, 255, 0);
        return color;
    }

    const Color& Color::BLUE() {
        static const Color color = Color::rgb(0, 0, 255);
        return color;
    }

    const Color& Color::WHITE() {
        static const Color color = Color::rgb(255, 255, 255);
        return color;
    }

    const Color& Color::BLACK() {
        static const Color color = Color::rgb(0, 0, 0);
        return color;
    }
}
