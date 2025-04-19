#include <algorithm>
#include <codecvt>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <regex>
#include <unicode/brkiter.h>
#include <unicode/uscript.h>
#include <unicode/utypes.h>
#include <corn/util/exceptions.h>
#include <corn/util/string_utils.h>

namespace corn {
    std::string trim(const std::string& str) noexcept {
        std::size_t start = str.find_first_not_of(WHITESPACE);

        // If str consists entirely of whitespace
        if (start == std::string::npos) {
            return "";
        }

        std::size_t end = str.find_last_not_of(WHITESPACE);
        return str.substr(start, end - start + 1);
    }

    // Function to split a string by multiple delimiters
    std::vector<std::string> split(const std::string& str, const std::string& delimiters) {
        std::vector<std::string> tokens;
        std::string token;
        for (char ch : str) {
            if (delimiters.find(ch) != std::string::npos) {
                if (!token.empty()) {
                    tokens.push_back(token);
                    token.clear();
                }
                tokens.emplace_back(1, ch); // Add the delimiter as a separate token
            } else {
                token += ch;
            }
        }
        if (!token.empty()) {
            tokens.push_back(token);
        }
        return tokens;
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

    size_t count(const std::string& str) noexcept {
        size_t num = 0;
        for (char c : str) {
            // Count only the starting bytes of characters
            if ((c & 0xC0) != 0x80) num++;
        }
        return num;
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

    std::string loadStringFromFile(const std::filesystem::path& file) {
        std::ifstream filestream(file);
        if (!filestream) {
            throw std::runtime_error("Could not open the file: '" + file.string() + "'");
        }

        std::stringstream buffer;
        buffer << filestream.rdbuf();
        return buffer.str();
    }

    std::string format(const std::string& fstring, const std::unordered_map<std::string, std::string>& dictionary) {
        std::string result = fstring;
        for (const auto& pair : dictionary) {
            // Create a regex pattern to find the placeholder in the format string
            std::string pattern = "\\$\\{" + pair.first + "\\}";
            std::regex re(pattern);

            // Replace all occurrences of the placeholder with the value from the dictionary
            result = std::regex_replace(result, re, pair.second);
        }
        return result;
    }
}
