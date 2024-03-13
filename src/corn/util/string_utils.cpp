#include <algorithm>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <unicode/brkiter.h>
#include <unicode/uscript.h>
#include <unicode/utypes.h>
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

    bool usesSpaceForWordBoundaries(UChar32 character) {
        UErrorCode error = U_ZERO_ERROR;
        UScriptCode script = uscript_getScript(character, &error);

        // Add more scripts here if needed
        return script == USCRIPT_LATIN || script == USCRIPT_CYRILLIC || script == USCRIPT_GREEK;
    }

    std::vector<std::u8string> breakIntoWords(const std::u8string& str) {
        std::vector<std::u8string> words;
        UErrorCode status = U_ZERO_ERROR;

        icu::UnicodeString unicodeStr = icu::UnicodeString::fromUTF8(
                icu::StringPiece(reinterpret_cast<const char*>(str.data()), (int32_t)str.size()));

        std::unique_ptr<icu::BreakIterator> wordIter(icu::BreakIterator::createWordInstance(icu::Locale::getDefault(), status));
        if (U_FAILURE(status)) {
            throw std::runtime_error(u_errorName(status));
        }

        wordIter->setText(unicodeStr);
        for (int32_t start = wordIter->first(), end = wordIter->next(); end != icu::BreakIterator::DONE; start = end, end = wordIter->next()) {
            icu::UnicodeString word = unicodeStr.tempSubStringBetween(start, end);

            UChar32 firstChar = 0;
            U16_GET(unicodeStr.getBuffer(), 0, start, unicodeStr.length(), firstChar);

            if (!usesSpaceForWordBoundaries(firstChar)) {
                // For scripts that don't use spaces, break each character into a word
                int32_t i = start;
                while (i < end) {
                    UChar32 ch;
                    int32_t prev_i = i;
                    U16_NEXT(unicodeStr.getBuffer(), i, end, ch);

                    icu::UnicodeString charWord = unicodeStr.tempSubStringBetween(prev_i, i);

                    std::string utf8Char;
                    charWord.toUTF8String(utf8Char);
                    words.emplace_back(reinterpret_cast<const char8_t*>(utf8Char.data()), utf8Char.length());
                }
            } else {
                // For scripts that use spaces, use the word as is
                std::string utf8Word;
                word.toUTF8String(utf8Word);
                words.emplace_back(reinterpret_cast<const char8_t*>(utf8Word.data()), utf8Word.length());
            }
        }

        return words;
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

    std::string loadStringFromFile(const std::string& file) {
        std::ifstream filestream(file);
        if (!filestream) {
            throw std::runtime_error("Could not open the file: '" + file + "'");
        }

        std::stringstream buffer;
        buffer << filestream.rdbuf();
        return buffer.str();
    }
}
