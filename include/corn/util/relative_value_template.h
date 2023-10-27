#pragma once

#include <stack>
#include "relative_value_helper.h"

namespace corn {
    namespace impl::rel_val {
        /**
         * @brief Breaks the input string into tokens.
         * @param expression The expression string to be calculated.
         * @param units Unordered map of all valid units to their index.
         * @return A vector of tokens.
         */
        std::vector<Token> tokenize(const std::string& expression, const std::unordered_map<std::string, size_t>& unitIdx) {
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
                    std::string temp = trim(expression.substr(tokenStart, i - tokenStart));
                    if (!temp.empty()) {
                        Token token(temp);
                        // Detect unrecognizable tokens
                        if (token.type == TokenType::INVALID ||
                            token.type == TokenType::VALUE && !(token.name.empty() || unitIdx.contains(token.name))
                        )
                            throw RelValParseFailed("Token `" + temp + "` cannot be recognized.");
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
                        // If is a function, push end function token to represent the end of argument list.
                        // Validate that it is succeeded by `(`.
                        temp.emplace_back(TokenType::END_FUNC, "", 0.0f, false);
                        operatorStack.push(token);
                        if (i + 1 == tokens.size() || tokens[i + 1].type != TokenType::PARENTHESIS_LEFT)
                            throw RelValParseFailed("`(` expected after function " + token.name + ".");
                        break;
                    case TokenType::PARENTHESIS_LEFT:
                        // If is `(`, push it to the operator stack.
                        // If previous  token is not function, add an `eval` function manually.
                        if (i == 0 || tokens[i - 1].type != TokenType::FUNCTION) {
                            temp.emplace_back(TokenType::END_FUNC, "", 0.0f, false);
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
                        // Push a separator before pushing the function
                        temp.emplace_back(TokenType::SEPARATOR, "", 0.0f, false);
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
                    case TokenType::VALUE:
                        // If is a value, let it fall through
                        result.push_back(token);
                        break;
                    case TokenType::SEPARATOR:
                        // If is `,`, push all operators until reaches end function token, then push itself.
                        // Validate that such `(` can be found.
                        while (true) {
                            if (operatorStack.empty()) {
                                std::cout << "<" << operatorStack.top().toString() << ">" << std::endl;
                                throw RelValParseFailed("Separator `,` used outside of function call.");
                            }
                            else if (operatorStack.top().type == TokenType::END_FUNC)
                                break;
                            // Keep moving operators from the stack to the result vector if not reached destination.
                            result.push_back(operatorStack.top());
                            operatorStack.pop();
                        }
                        // Push the operator itself to the stack
                        result.push_back(token);
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
                    case TokenType::END_FUNC:
                        // If is an end function token, push to both operator stack and result
                        result.push_back(token);
                        operatorStack.push(token);
                        break;
                    case TokenType::FUNCTION:
                        // If is a function, push itself and pop end function token from the stack.
                        // Function call must be preceded by a separator, which forces the top two of the stack to be
                        // the separator and the end function token.
                        result.push_back(token);
                        operatorStack.pop();
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
            std::stack<Token> valueStack;

            for (const Token& token : tokens) {
                switch (token.type) {
                    case TokenType::VALUE:
                        // If is value, push to stack.
                        valueStack.push(token);
                        break;
                    case TokenType::SEPARATOR: {
                        // If is separator, validate that there is exactly one value since last end function token or
                        // separator.
                        if (valueStack.size() < 2)
                            throw RelValParseFailed("Separator `,` used outside of function call.");
                        // Retrieve last two values and verify
                        Token token2 = valueStack.top();
                        valueStack.pop();
                        Token token1 = valueStack.top();  // No need to pop this. Can use it to store result.
                        if (token2.type != TokenType::VALUE)
                            throw RelValParseFailed("Invalid syntax of function arguments.");
                        // Push last value and new separator
                        if (token1.type == TokenType::SEPARATOR) {  // Remove the last separator
                            valueStack.top() = token2;
                            valueStack.push(token);
                        } else if (token1.type == TokenType::END_FUNC) {  // If this is the first separator, don't remove
                            valueStack.push(token2);
                            valueStack.push(token);
                        } else {
                            throw RelValParseFailed("Invalid syntax of function arguments.");
                        }
                        break;
                    }
                    case TokenType::OPERATOR: {
                        // If is operator, calculate the resulting unit.
                        if (valueStack.size() < 2)
                            throw RelValParseFailed("Binary operator `" + token.name + "` require two operands.");
                        // Retrieve last two values
                        Token token2 = valueStack.top();
                        valueStack.pop();
                        Token token1 = valueStack.top();  // No need to pop this. Can use it to store result.
                        if (token1.type != TokenType::VALUE || token2.type != TokenType::VALUE)
                            throw RelValParseFailed("Binary operators `" + token.name + "` require two operands.");
                        // Calculate and store result
                        bool result = _operators.at(token.name[0]).second(token1.value.hasUnit, token2.value.hasUnit);
                        valueStack.top().value.hasUnit = result;
                        break;
                    }
                    case TokenType::END_FUNC:
                        // If is end function token, push to stack.
                        valueStack.push(token);
                        break;
                    case TokenType::FUNCTION: {
                        // If is function, retrieve the list of arguments and calculate the resulting unit.
                        // The pairing of function call and function end token is already validated in toPostfix.
                        // First pop the separator
                        valueStack.pop();
                        // Retrieve values until reaching end
                        std::vector<bool> operands;
                        while (valueStack.top().type != TokenType::END_FUNC) {
                            operands.push_back(valueStack.top().value.hasUnit);
                            valueStack.pop();
                        }
                        std::reverse(operands.begin(), operands.end());
                        // Calculate and store result
                        bool result = _functions.at(token.name)(operands);
                        valueStack.top().type = TokenType::VALUE;
                        valueStack.top().value.hasUnit = result;
                        break;
                    }
                    default:
                        // Impossible
                        break;
                }
            }

            // Finally, check that the valueStack has one value left.
            if (valueStack.size() != 1 || valueStack.top().type != TokenType::VALUE)
                throw RelValParseFailed("Invalid expression.");
        }
    }

    template <std::size_t N>
    RelativeValue<N>::RelativeValue(): tokens(), unitIdx() {}

    template <std::size_t N>
    RelativeValue<N> RelativeValue<N>::fromString(const std::string& expression, const std::array<std::string, N>& units) {
        RelativeValue<N> result;

        // Convert units to unordered map
        for (size_t i = 0; i < N; i++) {
            result.unitIdx[units.at(i)] = i;
        }

        // Tokenization
        std::vector<impl::rel_val::Token> tokens = impl::rel_val::tokenize(expression, result.unitIdx);
        // Infix to postfix
        result.tokens = impl::rel_val::toPostfix(tokens);
        // Validation
        impl::rel_val::validate(result.tokens);

        return result;
    }

    template <std::size_t N>
    template <typename... Values> requires ValidRelValArgs<N, Values...>
    float RelativeValue<N>::calc(Values... unitValues) {
        std::array<float, N> valuesArray = { unitValues... };
        std::stack<impl::rel_val::Token> valueStack;

        for (const impl::rel_val::Token& token : tokens) {
            switch (token.type) {
                case impl::rel_val::TokenType::VALUE: {
                    // If is value, push to stack.
                    float newVal = token.value.hasUnit ?
                            token.value.val * valuesArray[this->unitIdx[token.name]] : token.value.val;
                    valueStack.emplace(token.type, "", newVal, token.value.hasUnit);
                    break;
                }
                case impl::rel_val::TokenType::SEPARATOR: {
                    // If is separator, skip.
                    break;
                }
                case impl::rel_val::TokenType::OPERATOR: {
                    // If is operator, calculate the resulting unit.
                    // Retrieve last two values
                    impl::rel_val::Token token2 = valueStack.top();
                    valueStack.pop();
                    impl::rel_val::Token token1 = valueStack.top();  // No need to pop this. Can use it to store result.
                    // Calculate and store result
                    valueStack.top().value = impl::rel_val::operators.at(token.name[0])(token1.value, token2.value);
                    break;
                }
                case impl::rel_val::TokenType::END_FUNC:
                    // If is end function token, push to stack.
                    valueStack.push(token);
                    break;
                case impl::rel_val::TokenType::FUNCTION: {
                    // If is function, retrieve the list of arguments and calculate the resulting unit.
                    // Retrieve values until reaching end
                    std::vector<impl::rel_val::Value> operands;
                    while (valueStack.top().type != impl::rel_val::TokenType::END_FUNC) {
                        operands.push_back(valueStack.top().value);
                        valueStack.pop();
                    }
                    std::reverse(operands.begin(), operands.end());
                    // Calculate and store result
                    valueStack.top().type = impl::rel_val::TokenType::VALUE;
                    valueStack.top().value = impl::rel_val::functions.at(token.name)(operands);
                    break;
                }
                default:
                    // Impossible
                    break;
            }
        }

        return valueStack.top().value.val;
    }
}
