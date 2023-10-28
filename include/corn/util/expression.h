#pragma once

#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>
#include <corn/geometry/vec2.h>

namespace corn {
    template <std::size_t N, typename... Values>
    concept ValidExpressionArgs = (sizeof...(Values) == N) && (std::same_as<float, Values> && ...);

    namespace impl::expression {
        struct Token;
    }

    /**
     * @class Expression
     * @tparam N Number of units
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
        Expression();
        static Expression<N> fromString(const std::string& expression, const std::array<std::string, N>& units);

        template <typename... Values> requires ValidExpressionArgs<N, Values...>
        float calc(Values... values);

    private:
        std::vector<impl::expression::Token> tokens;
        std::unordered_map<std::string, size_t> unitIdx;
    };
}

#include "expression_template.h"
