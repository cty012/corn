#pragma once

#include <corn/util/exceptions.h>

namespace corn {
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
     * @brief Helper function for `Expression::fromString`.
     * @param input The input string containing the expression.
     * @param unitIdx An unordered_map that maps units (as std::string) to their index that the user specifies.
     * @return A pointer to a vector containing the resulting tokens from analyzing the string.
     */
    std::vector<Token>* expressionFromStringImpl(
            const std::string& input, const std::unordered_map<std::string, size_t>& unitIdx);

    template <std::size_t N>
    Expression<N> Expression<N>::fromString(const std::string& input, const std::array<std::string, N>& units) {
        Expression<N> result;

        // Convert units to unordered map
        for (size_t i = 0; i < N; i++) {
            result.unitIdx[units.at(i)] = i;
        }

        result.tokens = expressionFromStringImpl(input, result.unitIdx);

        return result;
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
