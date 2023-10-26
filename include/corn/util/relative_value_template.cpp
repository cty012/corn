#pragma once

#include <algorithm>
#include <map>
#include <unordered_set>
#include <utility>
#include <corn/util/exceptions.h>
#include "relative_value_helper.cpp"

namespace corn {
    namespace impl::rel_val {
        /**
         * @brief Breaks the input string into tokens.
         * @param expression The expression string to be calculated.
         * @param units Unordered map of all valid units to their index.
         * @return A vector of tokens.
         */
        std::vector<Token> tokenize(const std::string& expression, const std::unordered_map<std::string, size_t>& units) {
            if (expression.empty())
                throw RelValParseFailed("Cannot parse empty expression.");

            std::vector<Token> result;
            size_t tokenStart = 0;

            // Move i past the end of the expression to parse the final token
            for (size_t i = 0; i <= expression.size(); i++) {
                // Check if end of string or is an operator
                if (i == expression.size() || reservedTokens.contains(expression[i]) ||
                    _operators.contains(expression[i])) {

                    // If there's a word before the operator, add it to the result
                    if (i > tokenStart) {
                        Token token(trim(expression.substr(tokenStart, i - tokenStart)));
                        // Detect unrecognizable tokens
                        if (token.type == TokenType::INVALID || token.type == TokenType::VALUE && !units.contains(token.name))
                            throw RelValParseFailed("Token `" + token.name + "` cannot be recognized.");
                        result.push_back(token);
                    }

                    // Process the current operator
                    if (i < expression.size()) {
                        result.emplace_back(expression.substr(i, 1));
                        tokenStart = i + 1;
                    }
                }
            }
            return result;
        }

        /**
         * @param tokens A vector of tokens in infix notation.
         * @return The same set of tokens but in postfix notation.
         *
         * Uses a variation of Shunting-yard algorithm to convert from infix to postfix.
         */
        std::vector<Token> toPostfix(const std::vector<Token>& tokens) {
            std::vector<Token> temp;
            std::vector<Token> result;
            std::stack<Token> operatorStack;

            // First pass: convert all functions and parentheses to postfix
            for (size_t i = 0; i < tokens.size(); i++) {
                const Token& token = tokens[i];
                switch (token.type) {
                    case TokenType::FUNCTION:
                        // If is a function, push empty function to represent the end of argument list.
                        // Validate that it is succeeded by `(`.
                        temp.emplace_back(TokenType::FUNCTION, "", 0.0f, false);
                        operatorStack.push(token);
                        if (i + 1 == tokens.size() || tokens[i + 1].type != TokenType::PARENTHESIS_LEFT)
                            throw RelValParseFailed("`(` expected after function " + token.name + ".");
                        break;
                    case TokenType::PARENTHESIS_LEFT:
                        // If is `(`, push it to the operator stack.
                        // If previous  token is not function, add an `eval` function manually.
                        if (i == 0 || tokens[i - 1].type != TokenType::FUNCTION) {
                            temp.emplace_back(TokenType::FUNCTION, "", 0.0f, false);
                            operatorStack.emplace(TokenType::FUNCTION, "eval", 0.0f, false);
                        }
                        // Push the `(`
                        operatorStack.push(token);
                        break;
                    case TokenType::PARENTHESIS_RIGHT:
                        // If is `)`, pop the `(` on the operator stack.
                        // Validate that the top of the stack is `(`.
                        if (operatorStack.empty() || operatorStack.top().type != TokenType::PARENTHESIS_LEFT)
                            throw RelValParseFailed("Token `)` is not paired with `(`.");
                        // Pop the `(`
                        operatorStack.pop();
                        // Push the function
                        temp.push_back(operatorStack.top());
                        operatorStack.pop();
                        break;
                    default:
                        // If other tokens, let them fall through.
                        temp.push_back(token);
                        break;
                }
            }

            // Verify that the operator stack is empty
            if (!operatorStack.empty()) {
                throw RelValParseFailed("Token `(` is not paired with `)`.");
            }

            // Second pass: convert all binary operators to postfix
            for (const Token& token : temp) {
                switch (token.type) {
                    case TokenType::SEPARATOR:
                        // If is `,`, push all operators until reaches end function token.
                        // Validate that such `(` can be found.
                        while (true) {
                            if (operatorStack.empty())
                                throw RelValParseFailed("Separator `,` used outside of function call.");
                            else if (isEndFunctionToken(operatorStack.top()))
                                break;
                            // Keep moving operators from the stack to the result vector if not reached destination.
                            result.push_back(operatorStack.top());
                            operatorStack.pop();
                        }
                        break;
                    case TokenType::FUNCTION:
                        // If is an end function token, push to both operator stack and result
                        if (isEndFunctionToken(token)) {
                            result.push_back(token);
                            operatorStack.push(token);
                            break;
                        }
                        // If is a function, push all operators until reaches end function token, then pop the token.
                        while (!isEndFunctionToken(operatorStack.top())) {
                            result.push_back(operatorStack.top());
                            operatorStack.pop();
                        }
                        result.push_back(token);
                        operatorStack.pop();
                        break;
                    case TokenType::OPERATOR:
                        // If is an operator:
                        while (!operatorStack.empty()) {
                            // Stop when the top of the stack is not an operator.
                            if (operatorStack.top().type != TokenType::OPERATOR)
                                break;
                            // Stop when the top of the stack has smaller precedence than the new operator.
                            if (precedes(token.name[0], operatorStack.top().name[0]))
                                break;
                            // Otherwise keep moving operators from the stack to the result vector.
                            result.push_back(operatorStack.top());
                            operatorStack.pop();
                        }
                        // Push the operator itself to the stack
                        operatorStack.push(token);
                        break;
                    case TokenType::VALUE:
                        // If is a value, let it fall through
                        result.push_back(token);
                        break;
                    default:
                        // Impossible
                        break;
                }
            }

            // Push remaining operators to the output queue.
            // Validate there are no `(` remaining on the operator stack.
            while (!operatorStack.empty()) {
                if (operatorStack.top().type == TokenType::PARENTHESIS_LEFT)
                    throw RelValParseFailed("Token `(` is not paired with `)`.");
                result.push_back(operatorStack.top());
                operatorStack.pop();
            }

            return result;
        }

        /**
         * @brief Validates whether the parsed expression has valid syntax.
         * @param tokens The vector of token to be validated.
         */
        void validate(const std::vector<Token>& tokens) {
            // TODO
        }
    }

    template <std::size_t N>
    RelativeValue<N>::RelativeValue(): tokens(), units() {}

    template <std::size_t N>
    RelativeValue<N> RelativeValue<N>::fromString(const std::string& expression, const std::array<std::string, N>& units) {
        RelativeValue<N> result;

        // Convert units to unordered map
        for (size_t i = 0; i < units; i++) {
            result->units[units[i]] = i;
        }

        // Tokenization
        std::vector<impl::rel_val::Token> tokens = impl::rel_val::tokenize(expression, result->units);

        // Infix to postfix
        result.tokens = impl::rel_val::toPostfix(tokens);

        // Validation
        impl::rel_val::validate(result.tokens);

        return result;
    }

    template <std::size_t N>
    template <typename... Values> requires ValidRelValArgs<N, Values...>
    float RelativeValue<N>::calc(Values... values) {
        std::array<float, N> valuesArray = { values... };
        float result = 0.0f;
        // TODO
        return result;
    }
}
