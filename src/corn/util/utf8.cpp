#include <cstdio>
#include <corn/util/exceptions.h>
#include <corn/util/utf8.h>

namespace corn {
    std::u8string unicodeToUTF8(unsigned int codepoint) {
        std::u8string result;

        if (codepoint <= 0x7F) {
            // 1-byte sequence
            result.push_back(static_cast<char8_t>(codepoint));
        } else if (codepoint <= 0x7FF) {
            // 2-byte sequence
            result.push_back(static_cast<char8_t>((codepoint >> 6) | 0xC0));
            result.push_back(static_cast<char8_t>((codepoint & 0x3F) | 0x80));
        } else if (codepoint <= 0xFFFF) {
            // 3-byte sequence
            result.push_back(static_cast<char8_t>((codepoint >> 12) | 0xE0));
            result.push_back(static_cast<char8_t>(((codepoint >> 6) & 0x3F) | 0x80));
            result.push_back(static_cast<char8_t>((codepoint & 0x3F) | 0x80));
        } else if (codepoint <= 0x10FFFF) {
            // 4-byte sequence
            result.push_back(static_cast<char8_t>((codepoint >> 18) | 0xF0));
            result.push_back(static_cast<char8_t>(((codepoint >> 12) & 0x3F) | 0x80));
            result.push_back(static_cast<char8_t>(((codepoint >> 6) & 0x3F) | 0x80));
            result.push_back(static_cast<char8_t>((codepoint & 0x3F) | 0x80));
        } else {
            // Invalid Unicode code point
            throw std::invalid_argument("Invalid Unicode code point");
        }

        return result;
    }

    void printu8(const std::u8string& target) {
        printf("%s", reinterpret_cast<const char*>(target.c_str()));
    }
}
