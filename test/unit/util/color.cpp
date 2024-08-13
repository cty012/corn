#include <gtest/gtest.h>
#include <corn/util/color.h>
#include "utility.h"

namespace corn::test::color {
    TEST(Color, parse) {
        corn::Color color, color2;
        Deg h;
        float s, l;
        unsigned char a;

        // White
        color = corn::Color::parse("#ffffff");
        EXPECT_EQ(color.hexStringAlpha(), "#ffffffff");
        std::tie(h, s, l, a) = color.getHSLA();
        EXPECT_FLOAT_EQ(h.get(), 0.0f);
        EXPECT_FLOAT_EQ(s, 0.0f);
        EXPECT_FLOAT_EQ(l, 100.0f);
        EXPECT_EQ(a, 255);
        color2 = corn::Color::hsl({ h, s, l, a });
        EXPECT_EQ(color.hexStringAlpha(), color2.hexStringAlpha());

        // Black
        color = corn::Color::parse("#000000");
        EXPECT_EQ(color.hexStringAlpha(), "#000000ff");
        std::tie(h, s, l, a) = color.getHSLA();
        EXPECT_FLOAT_EQ(h.get(), 0.0f);
        EXPECT_FLOAT_EQ(s, 0.0f);
        EXPECT_FLOAT_EQ(l, 0.0f);
        EXPECT_EQ(a, 255);
        color2 = corn::Color::hsl({ h, s, l, a });
        EXPECT_EQ(color.hexStringAlpha(), color2.hexStringAlpha());

        // Red
        color = corn::Color::RED();
        EXPECT_EQ(color.hexStringAlpha(), "#ff0000ff");
        std::tie(h, s, l, a) = color.getHSLA();
        EXPECT_FLOAT_EQ(h.get(), 0.0f);
        EXPECT_FLOAT_EQ(s, 100.0f);
        EXPECT_FLOAT_EQ(l, 50.0f);
        EXPECT_EQ(a, 255);
        color2 = corn::Color::hsl({ h, s, l, a });
        EXPECT_EQ(color.hexStringAlpha(), color2.hexStringAlpha());

        // Green
        color = corn::Color::GREEN();
        EXPECT_EQ(color.hexStringAlpha(), "#00ff00ff");
        std::tie(h, s, l, a) = color.getHSLA();
        EXPECT_FLOAT_EQ(h.get(), 120.0f);
        EXPECT_FLOAT_EQ(s, 100.0f);
        EXPECT_FLOAT_EQ(l, 50.0f);
        EXPECT_EQ(a, 255);
        color2 = corn::Color::hsl({ h, s, l, a });
        EXPECT_EQ(color.hexStringAlpha(), color2.hexStringAlpha());

        // Blue
        color = corn::Color::BLUE();
        EXPECT_EQ(color.hexStringAlpha(), "#0000ffff");
        std::tie(h, s, l, a) = color.getHSLA();
        EXPECT_FLOAT_EQ(h.get(), 240.0f);
        EXPECT_FLOAT_EQ(s, 100.0f);
        EXPECT_FLOAT_EQ(l, 50.0f);
        EXPECT_EQ(a, 255);
        color2 = corn::Color::hsl({ h, s, l, a });
        EXPECT_EQ(color.hexStringAlpha(), color2.hexStringAlpha());

        // Yellow
        color = corn::Color::parse("#ffff00");
        EXPECT_EQ(color.hexStringAlpha(), "#ffff00ff");
        std::tie(h, s, l, a) = color.getHSLA();
        EXPECT_FLOAT_EQ(h.get(), 60.0f);
        EXPECT_FLOAT_EQ(s, 100.0f);
        EXPECT_FLOAT_EQ(l, 50.0f);
        EXPECT_EQ(a, 255);
        color2 = corn::Color::hsl({ h, s, l, a });
        EXPECT_EQ(color.hexStringAlpha(), color2.hexStringAlpha());

        // Some random colors
        color = corn::Color::parse("#abcdef");
        EXPECT_EQ(color.hexStringAlpha(), "#abcdefff");
        std::tie(h, s, l, a) = color.getHSLA();
        color2 = corn::Color::hsl({ h, s, l, a });
        EXPECT_EQ(color.hexStringAlpha(), color2.hexStringAlpha());
    }
}
