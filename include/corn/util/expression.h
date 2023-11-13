#pragma once

#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>
#include <corn/geometry/vec2.h>
#include <corn/util/exceptions.h>

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
         * @throw ExpressionParseFailed Will throw if has invalid syntax or operator/function is called on invalid
         * number or types of operands.
         */
        Expression(const std::string& input, const std::array<std::string, N>& units);

        /**
         * @param values A pack of floating point numbers, each representing the value of the corresponding unit.
         * @return The result of plugging in the unit values into the expression.
         * @throw std::exception Some errors (such as division by zero) can only be detected at runtime.
         */
        template <typename... Values> requires ValidExpressionArgs<N, Values...>
        float calc(Values... values) const;

    private:
        std::vector<Token>* tokens;
        std::unordered_map<std::string, size_t> unitIdx;
    };

    std::vector<Token>* duplicateTokens(std::vector<Token>* tokens);
    void deleteTokens(std::vector<Token>* tokens);

    template <std::size_t N>
    Expression<N>::Expression(): tokens(nullptr), unitIdx() {}

    template<std::size_t N>
    Expression<N>::~Expression() {
        deleteTokens(this->tokens);
    }

    template<std::size_t N>
    Expression<N>::Expression(const Expression& other)
            : tokens(duplicateTokens(other.tokens)), unitIdx(other.unitIdx) {}

    template<std::size_t N>
    Expression<N>& Expression<N>::operator=(const Expression& other) {
        if (&other == this) return *this;
        deleteTokens(this->tokens);
        this->tokens = duplicateTokens(other.tokens);
        this->unitIdx = other.unitIdx;
        return *this;
    }

    template<std::size_t N>
    Expression<N>::Expression(Expression&& other) noexcept: tokens(other.tokens), unitIdx(std::move(other.unitIdx)) {
        other.tokens = nullptr;
    }

    template<std::size_t N>
    Expression<N>& Expression<N>::operator=(Expression&& other) noexcept {
        if (&other == this) return *this;
        deleteTokens(this->tokens);
        this->tokens = other.tokens;
        other.tokens = nullptr;
        this->unitIdx = other.unitIdx;
        return *this;
    }

    /**
     * @brief Helper function for `Expression(const std::string&, const std::array<std::string, N>&)`.
     * @param input The input string containing the expression.
     * @param unitIdx An unordered_map that maps units (as std::string) to their index that the user specifies.
     */
    std::vector<Token>* expressionFromStringImpl(
            const std::string& input, const std::unordered_map<std::string, size_t>& unitIdx);

    template <std::size_t N>
    Expression<N>::Expression(const std::string& input, const std::array<std::string, N>& units): tokens(nullptr), unitIdx() {
        // Convert units to unordered map
        for (size_t i = 0; i < N; i++) {
            this->unitIdx[units.at(i)] = i;
        }
        this->tokens = expressionFromStringImpl(input, this->unitIdx);
    }

    /**
     * @param tokens
     * @param unitIdx An unordered_map that maps units (as std::string) to their index that the user specifies.
     * @param valuesArray
     * @return The result of plugging in the unit values into the expression.
     */
    float expressionCalcImpl(
            const std::vector<Token>& tokens, const std::unordered_map<std::string, size_t>& unitIdx,
            const std::vector<float>& valuesArray);

    template <std::size_t N>
    template <typename... Values> requires ValidExpressionArgs<N, Values...>
    float Expression<N>::calc(Values... unitValues) const {
        if (this->tokens == nullptr) {
            throw std::logic_error(
                    "Invalid operation: The Expression object was default-constructed and is not fully initialized.");
        }
        std::vector<float> valuesArray = { unitValues... };
        return expressionCalcImpl(*this->tokens, this->unitIdx, valuesArray);
    }
}
