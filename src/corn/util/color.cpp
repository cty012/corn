#include <cstdio>
#include <regex>
#include <corn/util/color.h>

namespace corn {
    const Color& Color::WHITE() noexcept {
        static const Color color = { 255, 255, 255, 255 };
        return color;
    }

    const Color& Color::BLACK() noexcept {
        static const Color color = { 0, 0, 0, 255 };
        return color;
    }

    const Color& Color::RED() noexcept {
        static const Color color = { 255, 0, 0, 255 };
        return color;
    }

    const Color& Color::GREEN() noexcept {
        static const Color color = { 0, 255, 0, 255 };
        return color;
    }

    const Color& Color::BLUE() noexcept {
        static const Color color = { 0, 0, 255, 255 };
        return color;
    }

    Color::Color() noexcept : r_(255), g_(255), b_(255), a_(255) {}

    Color::Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a) noexcept
            : r_(r), g_(g), b_(b), a_(a) {}

    Color Color::rgb(unsigned char r, unsigned char g, unsigned char b, unsigned char a) noexcept {
        return { r, g, b, a };
    }

    Color Color::rgb(const Color::RGB& rgbValue) noexcept {
        auto [r, g, b] = rgbValue;
        return { r, g, b, 255 };
    }

    Color Color::rgb(const Color::RGBA &rgbValue) noexcept {
        auto [r, g, b, a] = rgbValue;
        return { r, g, b, a };
    }

    Color::RGB Color::getRGB() const noexcept {
        return { this->r_, this->g_, this->b_ };
    }

    Color::RGBA Color::getRGBA() const noexcept {
        return { this->r_, this->g_, this->b_, this->a_ };
    }

    Color Color::parse(const std::string& hexString) {
        static const std::regex color_regex(R"--(#?([a-fA-F0-9]{6})([a-fA-F0-9]{2})?)--");
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

    std::string Color::hexString() const noexcept {
        char hexStr[8];
        std::snprintf(hexStr, sizeof(hexStr), "#%02x%02x%02x", this->r_, this->g_, this->b_);
        return { hexStr };
    }

    std::string Color::hexStringAlpha() const noexcept {
        char hexStr[10];
        std::snprintf(hexStr, sizeof(hexStr), "#%02x%02x%02x%02x", this->r_, this->g_, this->b_, this->a_);
        return {hexStr};
    }
}
