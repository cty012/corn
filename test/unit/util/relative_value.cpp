#include <gtest/gtest.h>
#include <corn/util/relative_value.h>
#include <utility.h>

namespace corn::test::relative_value {
    using namespace corn::impl::rel_val;

    template <typename... Strings>
    requires ((std::is_same_v<Strings, std::string> || std::is_same_v<Strings, const char*>) && ...)
    std::vector<Token> constructTokenVector(Strings... tokens) {
        std::vector<Token> result;
        (
            result.push_back(
                std::string(tokens).empty() ?
                Token(TokenType::END_FUNC, "", 0.0f, false) :
                Token(std::string(tokens))
            ),
            ...
        );
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

    bool _TestTokenVectorsEqual(const std::vector<Token>& tokens1, const std::vector<Token>& tokens2) {
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

    bool TestTokenVectorsEqual(const std::vector<Token>& tokens1, const std::vector<Token>& tokens2) {
        bool result = _TestTokenVectorsEqual(tokens1, tokens2);
        if (!result) {
            std::cout << "  tokens1: {" << std::endl << "    ";
            for (const Token& token1: tokens1) std::cout << token1.toString() << " ";
            std::cout << std::endl << "  }" << std::endl;
            std::cout << "  tokens2: {" << std::endl << "    ";
            for (const Token& token2: tokens2) std::cout << token2.toString() << " ";
            std::cout << std::endl << "  }" << std::endl;
        }
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

        Token pureFloat1("123.25");
        EXPECT_EQ(pureFloat1.type, TokenType::VALUE);
        EXPECT_EQ(pureFloat1.name, "");
        EXPECT_FLOAT_EQ(pureFloat1.value.val, 123.25f);
        EXPECT_EQ(pureFloat1.value.hasUnit, false);

        Token pureFloat2(".20");
        EXPECT_EQ(pureFloat2.type, TokenType::VALUE);
        EXPECT_EQ(pureFloat2.name, "");
        EXPECT_FLOAT_EQ(pureFloat2.value.val, 0.2f);
        EXPECT_EQ(pureFloat2.value.hasUnit, false);

        Token pxValue(".25px");
        EXPECT_EQ(pxValue.type, TokenType::VALUE);
        EXPECT_EQ(pxValue.name, "px");
        EXPECT_FLOAT_EQ(pxValue.value.val, 0.25f);
        EXPECT_EQ(pxValue.value.hasUnit, true);

        Token value1("192.168.1.1");
        EXPECT_EQ(value1.type, TokenType::VALUE);
        EXPECT_EQ(value1.name, ".1.1");
        EXPECT_FLOAT_EQ(value1.value.val, 192.168f);
        EXPECT_EQ(value1.value.hasUnit, true);

        Token invalid1("px");
        EXPECT_EQ(invalid1.type, TokenType::INVALID);
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

    TEST(RelativeValue, convert_to_postfix) {
        std::array<std::string, 3> units = {"px", "%w", "%h"};
        std::unordered_map<std::string, size_t> unitIdx;
        for (size_t i = 0; i < units.size(); i++) {
            unitIdx[units[i]] = i;
        }
        std::vector<Token> result;

        EXPECT_NO_THROW(result = toPostfix(tokenize("10px + 5%w", unitIdx)));
        TestTokenVectorsEqual(result, constructTokenVector("10px", "5%w", "+"));
        EXPECT_NO_THROW(result = toPostfix(tokenize("28%h + 2 * (10px + 5%w)", unitIdx)));
        TestTokenVectorsEqual(result, constructTokenVector(
                "28%h", "2", "", "10px", "5%w", "+", ",", "eval", "*", "+"));
    }

    TEST(RelativeValue, calculate) {
        std::array<std::string, 3> units = {"px", "%w", "%h"};
        RelativeValue<3> result;

        // Basic calculation
        EXPECT_NO_THROW(result = RelativeValue<3>::fromString("10px + 5%w", units));
        EXPECT_FLOAT_EQ(result.calc(1.0f, 3.0f, 0.0f), 25.0f);
        EXPECT_NO_THROW(result = RelativeValue<3>::fromString("28%h + 2 * (10px + 5%w)", units));
        EXPECT_FLOAT_EQ(result.calc(1.0f, 3.0f, 1.5f), 92.0f);

        // Unit manipulation
        EXPECT_NO_THROW(result = RelativeValue<3>::fromString("36px * (7px / 4.5%w)", units));
        EXPECT_FLOAT_EQ(result.calc(10.0f, 5.0f, 0.0f), 1120.0f);

        // Functions
        EXPECT_NO_THROW(result = RelativeValue<3>::fromString("min(100%w, 100%h * 16/9, 2000px) - 16px", units));
        EXPECT_FLOAT_EQ(result.calc(1.0f, 19.20f, 10.90f), 1904.0f);
        EXPECT_FLOAT_EQ(result.calc(1.0f, 19.20f, 1.080f), 176.0f);
        EXPECT_NO_THROW(result = RelativeValue<3>::fromString("min(5, 7, 3, (0-6)*2, 1*3*2, 4, 2/0.6) + 1.2*3.4", units));
        EXPECT_FLOAT_EQ(result.calc(0.0f, 0.0f, 0.0f), -7.92f);

        // Nested functions
        EXPECT_NO_THROW(result = RelativeValue<3>::fromString("max(min(100%w, 720px), 0px)", units));
        EXPECT_FLOAT_EQ(result.calc(1.0f, -1.0f, 0.0f), 0.0f);
        EXPECT_FLOAT_EQ(result.calc(1.0f, 1.1f, 0.0f), 110.0f);
        EXPECT_FLOAT_EQ(result.calc(1.0f, 10.0f, 0.0f), 720.0f);

        // Unary operators not supported
        EXPECT_THROW(RelativeValue<3>::fromString("-10px", units), RelValParseFailed);
        EXPECT_THROW(RelativeValue<3>::fromString("1 + -1", units), RelValParseFailed);

        // Invalid function arguments
        // `max` needs at least one argument
        EXPECT_THROW(RelativeValue<3>::fromString("max()", units), RelValParseFailed);
        // `min` requires arguments to have the same unit (px, %w, and %h are all considered as length unit)
        EXPECT_THROW(RelativeValue<3>::fromString("min(2, 3px)", units), RelValParseFailed);
        // Parentheses are Interpreted as the `eval` function, which requires 1 argument of any unit.
        EXPECT_THROW(RelativeValue<3>::fromString("(1, 2)", units), RelValParseFailed);
        EXPECT_THROW(RelativeValue<3>::fromString("eval()", units), RelValParseFailed);
    }
}
