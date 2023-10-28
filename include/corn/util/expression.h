#pragma once

#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>
#include <corn/geometry/vec2.h>

namespace corn {
    template <std::size_t N, typename... Values>
    concept ValidExpressionArgs = (sizeof...(Values) == N) && (std::same_as<float, Values> && ...);

    struct Token;

    /**
     * @class Expression
     * @tparam N Number of units.
     * @brief Holds an algebra expression. Can calculate the exact value by specifying the value of each unit.
     *
     * Supports:
     * 1. Binary operators +, -, *, /
     * 2. Functions min, max (with at least one argument)
     * 3. Can use parentheses to prioritize part of the expression.
     * 4. Can combine and nest the operators and functions to create very complex expressions.
     *
     * E.g. In `min(100px, 10%w)`, if set `px` to 1 and `%w` to 1080, the result would be 100.
     *
     * Note that values are categorized into number literals and numbers with units. Some operations such as adding a
     * literal with a unit is not supported.
     */
    template <std::size_t N>
    class Expression {
    public:
        // Ctor, dtor, copy/move constructors, and assignment operators.
        Expression();
        ~Expression();
        Expression(const Expression& other);
        Expression& operator=(const Expression& other);
        Expression(Expression&& other) noexcept;
        Expression& operator=(Expression&& other) noexcept;

        /**
         * @param input The input string containing the expression.
         * @param units An array of valid units.
         * @return The Expression object constructed from the string.
         * @throw ExpressionParseFailed Will throw if has invalid syntax or operator/function is called on invalid
         * number or types of operands.
         */
        static Expression<N> fromString(const std::string& input, const std::array<std::string, N>& units);

        /**
         * @param values A pack of floating point numbers, each representing the value of the corresponding unit.
         * @return The result of plugging in the unit values into the expression.
         * @throw std::exception Some errors (such as division by zero) can only be detected at runtime.
         */
        template <typename... Values> requires ValidExpressionArgs<N, Values...>
        float calc(Values... values);

    private:
        std::vector<Token>* tokens;
        std::unordered_map<std::string, size_t> unitIdx;
    };
}

#include "expression_template.h"
