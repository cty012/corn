#pragma once

#include <array>
#include <string>

namespace corn {
    inline const std::string WHITESPACE = "\t\n\v\f\r ";

    inline const std::array<const std::u8string, 5> WHITESPACE_UTF8 = {
            u8"\u2002", u8"\u2003", u8"\u2009", u8"\u200B", u8"\u3000"
    };

    /**
     * @param target Target character.
     * @return Whether the character is a whitespace character.
     */
    bool isWhitespace(char target) noexcept;

    /**
     * @param target Target UTF-8 character.
     * @return Whether the character is a whitespace character.
     */
    bool isWhitespace(const std::u8string& target) noexcept;

    /**
     * @brief Remove spaces and new lines from the begin and end of a string.
     * @param str Target string to trim.
     * @return Trimmed string.
     */
    std::string trim(const std::string& str) noexcept;

    /**
     * @brief Encode a unicode character in UTF-8.
     * @param codepoint The unicode number.
     * @return The encoded UTF-8 string.
     */
    std::u8string unicodeToUTF8(unsigned int codepoint);

    /**
     * @param str Target UTF-8 encoded string.
     * @return The first UTF-8 character in the string.
     */
    std::u8string getChar(const char8_t* str) noexcept;

    /**
     * @param str Target UTF-8 encoded string.
     * @return The first UTF-8 character in the string.
     */
    std::u8string getChar(const std::u8string& str) noexcept;

    /**
     * @brief Count the number of UTF-8 characters in the string.
     * @param str The UTF-8 string.
     * @return The number of UTF-8 characters.
     */
    size_t count(const std::u8string& str) noexcept;

    /**
     * @brief Print a UTF-8 string onto the console.
     * @param str The UTF-8 string to display.
     *
     * Before calling this function, make sure the console is capable of displaying UTF-8 text.
     */
    void printu8(const std::u8string& str);
}
