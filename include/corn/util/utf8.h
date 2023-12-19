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
     * @brief Print a UTF-8 string onto the console.
     * @param target The UTF-8 string to display.
     *
     * Before calling this function, make sure the console is capable of displaying UTF-8 text.
     */
    void printu8(const std::u8string& target);
}
