#include <gtest/gtest.h>
#include <corn/util/relative_value.h>
#include <utility.h>

namespace corn::test::relative_value {
    using namespace corn::impl::rel_val;

    template <typename... Strings>
    requires ((std::is_same_v<Strings, std::string> || std::is_same_v<Strings, const char*>) && ...)
    std::vector<Token> constructTokenVector(Strings... tokens) {
        std::vector<Token> result;
        (result.emplace_back(std::string(tokens)), ...);
        return result;
    }

    bool TestTokensEqual(const Token& token1, const Token& token2) {
        EXPECT_EQ_RETURN(token1.type, token2.type, false);
        switch (token1.type) {
            case TokenType::PARENTHESIS_LEFT:
            case TokenType::PARENTHESIS_RIGHT:
            case TokenType::SEPARATOR:
            case TokenType::INVALID:
                return true;
            case TokenType::VALUE:
                EXPECT_EQ_RETURN(token1.name, token2.name, false);
                EXPECT_FLOAT_EQ_RETURN(token1.value.val, token2.value.val, false);
                EXPECT_EQ_RETURN(token1.value.hasUnit, token2.value.hasUnit, false);
                return true;
            case TokenType::OPERATOR:
                EXPECT_EQ_RETURN(token1.name, token2.name, false);
                return true;
            case TokenType::FUNCTION:
                EXPECT_EQ_RETURN(token1.name, token2.name, false);
                return true;
        }
        return true;
    }

    bool TestTokenVectorsEqual(const std::vector<Token>& tokens1, const std::vector<Token>& tokens2) {
        EXPECT_EQ_RETURN(tokens1.size(), tokens2.size(), false);
        bool result = true;
        for (size_t i = 0; i < tokens1.size(); i++) {
            bool temp = TestTokensEqual(tokens1[i], tokens2[i]);
            EXPECT_TRUE(temp);
            if (!temp) result = false;
        }
        EXPECT_TRUE(result);
        return result;
    }

    TEST(RelativeValue, token_is_interpreted_correctly) {
        Token parenthesisLeft("(");
        EXPECT_EQ(parenthesisLeft.type, TokenType::PARENTHESIS_LEFT);

        Token parenthesisRight(")");
        EXPECT_EQ(parenthesisRight.type, TokenType::PARENTHESIS_RIGHT);

        Token separator(",");
        EXPECT_EQ(separator.type, TokenType::SEPARATOR);

        Token func1("min");
        EXPECT_EQ(func1.type, TokenType::FUNCTION);
        EXPECT_EQ(func1.name, "min");

        Token func2("eval");
        EXPECT_EQ(func2.type, TokenType::FUNCTION);
        EXPECT_EQ(func2.name, "eval");

        Token pureInt1("5");
        EXPECT_EQ(pureInt1.type, TokenType::VALUE);
        EXPECT_EQ(pureInt1.name, "");
        EXPECT_FLOAT_EQ(pureInt1.value.val, 5.0f);
        EXPECT_EQ(pureInt1.value.hasUnit, false);

        Token pureInt2("100");
        EXPECT_EQ(pureInt2.type, TokenType::VALUE);
        EXPECT_EQ(pureInt2.name, "");
        EXPECT_FLOAT_EQ(pureInt2.value.val, 100.0f);
        EXPECT_EQ(pureInt2.value.hasUnit, false);

        Token pureFloat("123.25");
        EXPECT_EQ(pureFloat.type, TokenType::VALUE);
        EXPECT_EQ(pureFloat.name, "");
        EXPECT_FLOAT_EQ(pureFloat.value.val, 123.25f);
        EXPECT_EQ(pureFloat.value.hasUnit, false);

        Token pxValue(".25px");
        EXPECT_EQ(pxValue.type, TokenType::VALUE);
        EXPECT_EQ(pxValue.name, "px");
        EXPECT_FLOAT_EQ(pxValue.value.val, 0.25f);
        EXPECT_EQ(pxValue.value.hasUnit, true);

        Token invalid("px");
        EXPECT_EQ(invalid.type, TokenType::INVALID);
    }

    TEST(RelativeValue, tokenization) {
        std::vector<std::string> units = {"px", "%w", "%h"};
        std::unordered_map<std::string, size_t> unitIdx;
        for (size_t i = 0; i < units.size(); i++) {
            unitIdx[units[i]] = i;
        }
        std::vector<Token> result;

        // Basic tokenization
        EXPECT_NO_THROW(result = tokenize("min(5px - 10.6, 10%w)", unitIdx));
        TestTokenVectorsEqual(result, constructTokenVector(
                "min", "(", "5px", "-", "10.6", ",", "10%w", ")"));

        // Single token
        EXPECT_NO_THROW(result = tokenize("eval", unitIdx));
        TestTokenVectorsEqual(result, constructTokenVector("eval"));

        // Multiple tokens
        EXPECT_NO_THROW(result = tokenize("min(max(3 * 4, 5), eval(1024 / 6.px))", unitIdx));
        TestTokenVectorsEqual(result, constructTokenVector(
                "min", "(", "max", "(", "3", "*", "4", ",", "5", ")", ",", "eval", "(", "1024", "/", "6.px", ")", ")"));

        // Nested functions
        EXPECT_NO_THROW(result = tokenize("eval(min(3, 4), max(5, 6))", unitIdx));
        TestTokenVectorsEqual(result, constructTokenVector(
                "eval", "(", "min", "(", "3", ",", "4", ")", ",", "max", "(", "5", ",", "6", ")", ")"));

        // Empty string
        EXPECT_THROW(tokenize("", unitIdx), RelValParseFailed);

        // Invalid tokens
        EXPECT_THROW(tokenize("hello world", unitIdx), RelValParseFailed);
        EXPECT_THROW(tokenize("1 px", unitIdx), RelValParseFailed);

        // Whitespace handling
        EXPECT_NO_THROW(result = tokenize(" min ( 5px , 10 ) ", unitIdx));
        TestTokenVectorsEqual(result, constructTokenVector("min", "(", "5px", ",", "10", ")"));

        // No space between tokens
        EXPECT_NO_THROW(result = tokenize("min(,3,4,)", unitIdx));
        TestTokenVectorsEqual(result, constructTokenVector("min", "(", ",", "3", ",", "4", ",", ")"));
        EXPECT_NO_THROW(result = tokenize("min(max(3,4),5)", unitIdx));
        TestTokenVectorsEqual(result, constructTokenVector(
                "min", "(", "max", "(", "3", ",", "4", ")", ",", "5", ")"));

        // Consecutive comma
        EXPECT_NO_THROW(result = tokenize("min(10,,20)", unitIdx));
        TestTokenVectorsEqual(result, constructTokenVector("min", "(", "10", ",", ",", "20", ")"));

        // Units without numbers
        EXPECT_THROW(tokenize("max(px, %w)", unitIdx), RelValParseFailed);

        // Numbers with multiple decimal points
        EXPECT_THROW(tokenize("10.5.3", unitIdx), RelValParseFailed);
    }

    TEST(RelativeValue, calculate) {
        std::array<std::string, 3> units = {"px", "%w", "%h"};

        // Basic calculation
        RelativeValue<3> result;
        EXPECT_NO_THROW(result = RelativeValue<3>::fromString("10px + 5%w", units));
        EXPECT_EQ(result.calc(1.0f, 3.0f, 0.0f), 25);
    }
}
