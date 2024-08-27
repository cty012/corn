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

    Color Color::rgb(const Color::RGB& rgbValues) noexcept {
        auto [r, g, b] = rgbValues;
        return { r, g, b, 255 };
    }

    Color Color::rgb(const Color::RGBA &rgbaValues) noexcept {
        auto [r, g, b, a] = rgbaValues;
        return { r, g, b, a };
    }

    Color Color::hsl(const HSL& hslValues) noexcept {
        auto [h, s, l] = hslValues;
        return Color::hsl({ h, s, l, 255 });
    }

    Color Color::hsl(const HSLA& hslaValues) noexcept {
        auto [h_, s, l, a] = hslaValues;

        // Validate input
        if (s < 0.0f) {
            s = 0.0f;
        } else if (s > 100.0f) {
            s = 100.0f;
        }
        if (l < 0.0f) {
            l = 0.0f;
        } else if (l > 100.0f) {
            l = 100.0f;
        }

        // Normalize
        float h = h_.get();
        s *= 0.01f;
        l *= 0.01f;

        // Chroma, hue prime, X, and m
        float c = (1 - std::abs(2 * l - 1)) * s;
        float hp = h / 60.0f;
        float x = c * (1 - std::abs(std::fmod(hp, 2.0f) - 1));
        float m = l - c * 0.5f;

        // RGB intermediate values
        float r_, g_, b_;
        if (hp >= 0 && hp < 1) {
            std::tie(r_, g_, b_) = std::make_tuple(c, x, 0.0f);
        } else if (hp >= 1 && hp < 2) {
            std::tie(r_, g_, b_) = std::make_tuple(x, c, 0.0f);
        } else if (hp >= 2 && hp < 3) {
            std::tie(r_, g_, b_) = std::make_tuple(0.0f, c, x);
        } else if (hp >= 3 && hp < 4) {
            std::tie(r_, g_, b_) = std::make_tuple(0.0f, x, c);
        } else if (hp >= 4 && hp < 5) {
            std::tie(r_, g_, b_) = std::make_tuple(x, 0.0f, c);
        } else {
            std::tie(r_, g_, b_) = std::make_tuple(c, 0.0f, x);
        }

        // RGB
        auto r = (unsigned char)((r_ + m) * 255.0f);
        auto g = (unsigned char)((g_ + m) * 255.0f);
        auto b = (unsigned char)((b_ + m) * 255.0f);

        return { r, g, b, a };
    }

    Color::RGB Color::getRGB() const noexcept {
        return { this->r_, this->g_, this->b_ };
    }

    Color::RGBA Color::getRGBA() const noexcept {
        return { this->r_, this->g_, this->b_, this->a_ };
    }

    Color::HSL Color::getHSL() const noexcept {
        float r = (float)this->r_ / 255.0f;
        float g = (float)this->g_ / 255.0f;
        float b = (float)this->b_ / 255.0f;

        // Chroma, hue prime, and X
        float cmax = std::max(r, std::max(g, b));
        float cmin = std::min(r, std::min(g, b));
        float c = cmax - cmin;
        float hp;
        if (c == 0) {
            hp = 0;
        } else if (cmax == r) {
            hp = std::fmod((g - b) / c, 6.0f);
        } else if (cmax == g) {
            hp = (b - r) / c + 2;
        } else {
            hp = (r - g) / c + 4;
        }

        // HSL
        float h = hp * 60;
        if (h < 0) {
            h += 360;
        }
        float l = (cmax + cmin) * 0.5f;
        float s = c == 0 ? 0 : c / (1 - std::abs(2 * l - 1));

        return { Deg(h), s * 100.0f, l * 100.0f };
    }

    Color::HSLA Color::getHSLA() const noexcept {
        auto [h, s, l] = this->getHSL();
        return { h, s, l, this->a_ };
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
