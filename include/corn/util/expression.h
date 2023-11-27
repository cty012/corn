/**
 * @file expression.h
 * @brief Defines the Expression class.
 */

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
     * literal with a unit are not supported.
     */
    template <std::size_t N>
    class Expression {
    public:
        /// @brief Constructor.
        Expression();
        /// @brief Destructor.
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
        std::string input_;
        std::vector<Token>* tokens_;
        std::unordered_map<std::string, size_t> unitIdx_;
    };

    /**
     * @brief Helper function for duplicating a vector of tokens.
     * @param tokens The vector of tokens to be duplicated.
     * @return A pointer to a copy of the original vector.
     */
    std::vector<Token>* duplicateTokens(std::vector<Token>* tokens);

    /**
     * @brief Helper function for deallocating a vector of tokens.
     * @param tokens The vector of tokens to be deallocated.
     */
    void deleteTokens(std::vector<Token>* tokens);

    template <std::size_t N>
    Expression<N>::Expression() : input_(), tokens_(nullptr), unitIdx_() {}

    template<std::size_t N>
    Expression<N>::~Expression() {
        deleteTokens(this->tokens_);
    }

    template<std::size_t N>
    Expression<N>::Expression(const Expression& other)
            : input_(other.input_), tokens_(duplicateTokens(other.tokens_)), unitIdx_(other.unitIdx_) {}

    template<std::size_t N>
    Expression<N>& Expression<N>::operator=(const Expression& other) {
        if (&other == this) return *this;
        deleteTokens(this->tokens_);
        this->input_ = other.input_;
        this->tokens_ = duplicateTokens(other.tokens_);
        this->unitIdx_ = other.unitIdx_;
        return *this;
    }

    template<std::size_t N>
    Expression<N>::Expression(Expression&& other) noexcept
            : input_(other.input_), tokens_(other.tokens), unitIdx_(std::move(other.unitIdx)) {

        other.tokens_ = nullptr;
    }

    template<std::size_t N>
    Expression<N>& Expression<N>::operator=(Expression&& other) noexcept {
        if (&other == this) return *this;
        deleteTokens(this->tokens_);
        this->input_ = other.input_;
        this->tokens_ = other.tokens_;
        other.tokens_ = nullptr;
        this->unitIdx_ = other.unitIdx_;
        return *this;
    }

    /**
     * @brief Helper function that parses an input string into a vector of tokens in postfix order.
     * @param input The input string containing the expression.
     * @param unitIdx An unordered_map that maps units (as std::string) to their index that the user specifies.
     * @return A vector of tokens parsed from the input string.
     * @throw ExpressionParseFailed If the expression has a syntax error or unit mismatch.
     */
    std::vector<Token>* expressionFromStringImpl(
            const std::string& input, const std::unordered_map<std::string, size_t>& unitIdx);

    template <std::size_t N>
    Expression<N>::Expression(const std::string& input, const std::array<std::string, N>& units)
            : input_(input), tokens_(nullptr), unitIdx_() {

        // Convert units to unordered map
        for (size_t i = 0; i < N; i++) {
            this->unitIdx_[units.at(i)] = i;
        }
        this->tokens_ = expressionFromStringImpl(input, this->unitIdx_);
    }

    /**
     * @brief Helper function for calculating the value of the expression.
     * @param tokens A vector of tokens in postfix order.
     * @param unitIdx An unordered_map that maps units (as std::string) to their index that the user specifies.
     * @param valuesArray An array of floating point numbers representing the value of each unit.
     * @return The result of plugging in the unit values into the expression.
     * @throw std::logic_error If the expression is uninitialized.
     */
    float expressionCalcImpl(
            const std::vector<Token>& tokens, const std::unordered_map<std::string, size_t>& unitIdx,
            const std::vector<float>& valuesArray);

    template <std::size_t N>
    template <typename... Values> requires ValidExpressionArgs<N, Values...>
    float Expression<N>::calc(Values... unitValues) const {
        if (this->tokens_ == nullptr) {
            throw std::logic_error(
                    "Invalid operation: The Expression object was default-constructed and is not fully initialized.");
        }
        std::vector<float> valuesArray = { unitValues... };
        return expressionCalcImpl(*this->tokens_, this->unitIdx_, valuesArray);
    }
}
