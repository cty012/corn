#include <gtest/gtest.h>
#include <corn/util/string_utils.h>
#include "utility.h"

namespace corn::test::string_utils {
    TEST(StringUtils, trim) {
        std::string str;

        // Empty string
        str = "";
        EXPECT_EQ(trim(str), "");

        // Spaces only
        str = "   \r\n    \n";
        EXPECT_EQ(trim(str), "");

        // Spaces in the front
        str = " Hello";
        EXPECT_EQ(trim(str), "Hello");

        // Spaces at the back
        str = "world!\n";
        EXPECT_EQ(trim(str), "world!");

        // Spaces on both sides
        str = " Helloworld!\n";
        EXPECT_EQ(trim(str), "Helloworld!");

        // Spaces in the middle
        str = " Hello\tworld!\n";
        EXPECT_EQ(trim(str), "Hello\tworld!");
    }

    TEST(StringUtils, break_into_words) {
        std::u8string str;
        std::vector<std::u8string> ans;

        // Empty string
        str = u8"";
        ans = { };
        VectorsEqual(breakIntoWords(str), ans);

        // Space only
        str = u8" ";
        ans = { u8" " };
        VectorsEqual(breakIntoWords(str), ans);

        // Spaces only
        str = u8"\r\n";
        ans = { u8"\r", u8"\n" };
        VectorsEqual(breakIntoWords(str), ans);
        str = u8"a  \tb";
        ans = { u8"a", u8" ", u8" ", u8"\t", u8"b" };
        VectorsEqual(breakIntoWords(str), ans);

        // Pure ASCII
        str = u8"abc defg\v \thij-klm";
        ans = { u8"abc", u8" ", u8"defg", u8"\v", u8" ", u8"\t", u8"hij", u8"-", u8"klm" };
        VectorsEqual(breakIntoWords(str), ans);

        // Chinese
        str = u8"你好，世界！";
        ans = { u8"你", u8"好", u8"，", u8"世", u8"界", u8"！" };
        VectorsEqual(breakIntoWords(str), ans);

        // Mixed
        str = u8"我like吃corn，我don't like吃pizza和kfc。";
        ans = {
                u8"我", u8"like", u8"吃", u8"corn", u8"，", u8"我", u8"don't", u8" ", u8"like", u8"吃",
                u8"pizza", u8"和", u8"kfc", u8"。"
        };
        VectorsEqual(breakIntoWords(str), ans);
    }
}
