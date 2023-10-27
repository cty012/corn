#include <string>
#include <gtest/gtest.h>

TEST(HelloWorldTest, ShouldPass) {
    std::string hello = "Hello, world!";
    EXPECT_EQ(hello, "Hello, world!");
}
