#include <gtest/gtest.h>
#include <corn/util/color.h>
#include "utility.h"

namespace corn::test::color {
    TEST(Color, parse) {
        corn::Color color;

        color = corn::Color::parse("#ffffff");
        EXPECT_EQ(color.hexStringAlpha(), "#ffffffff");
    }
}
