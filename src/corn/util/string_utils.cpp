#include <algorithm>
#include <cstdio>
#include <corn/util/exceptions.h>
#include <corn/util/string_utils.h>

namespace corn {
    bool isWhitespace(char target) noexcept {
        return std::any_of(WHITESPACE.begin(), WHITESPACE.end(), [target](char c) {
            return target == c;
        });
    }

    bool isWhitespace(const std::u8string& target) noexcept {
        if (target.size() == 1) {
            for (char c : WHITESPACE) {
                if (c == target[0]) return true;
            }
        }
        return std::any_of(WHITESPACE_UTF8.begin(), WHITESPACE_UTF8.end(), [target](const std::u8string& c) {
            return target == c;
        });
    }

    std::string trim(const std::string& str) noexcept {
        std::size_t start = str.find_first_not_of(WHITESPACE);

        // If str consists entirely of whitespace
        if (start == std::string::npos) {
            return "";
        }

        std::size_t end = str.find_last_not_of(WHITESPACE);
        return str.substr(start, end - start + 1);
    }

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

    std::u8string getChar(const char8_t* str) noexcept {
        std::u8string result;
        for (; *str; ++str) {
            if (!result.empty() && (*str & 0xC0) != 0x80) return result;
            result += *str;
        }
        return result;
    }

    std::u8string getChar(const std::u8string& str) noexcept {
        return getChar(str.c_str());
    }

    size_t count(const std::u8string& str) noexcept {
        size_t num = 0;
        for (char8_t c : str) {
            // Count only the starting bytes of characters
            if ((c & 0xC0) != 0x80) num++;
        }
        return num;
    }

    void printu8(const std::u8string& str) {
        printf("%s", reinterpret_cast<const char*>(str.c_str()));
    }
}
