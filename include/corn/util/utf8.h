#pragma once

#include <string>

namespace corn {
    /**
     * @brief Encode a unicode character in UTF-8.
     * @param codepoint The unicode number.
     * @return The encoded UTF-8 string.
     */
    std::u8string unicodeToUTF8(unsigned int codepoint);

    /**
     * @brief Count the number of UTF-8 characters in the string.
     * @param str The UTF-8 string.
     * @return The number of UTF-8 characters.
     */
    size_t count(const std::u8string& str);

    /**
     * @brief Print a UTF-8 string onto the console.
     * @param str The UTF-8 string to display.
     *
     * Before calling this function, make sure the console is capable of displaying UTF-8 text.
     */
    void printu8(const std::u8string& str);
}
