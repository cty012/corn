#include <stack>
#include <corn/util/expression.h>
#include "expression_impl.h"

namespace corn::impl::expression {
    std::vector<Token> tokenize(const std::string& input, const std::unordered_map<std::string, size_t>& unitIdx) {
        // Check not empty string
        if (input.empty()) {
            throw ExpressionParseFailed(input, "Empty expression.");
        }

        // Storing the list of tokens in infix notation.
        std::vector<Token> result;
        size_t tokenStart = 0;

        // Move i past the end of the expression to parse the final token
        for (size_t i = 0; i <= input.size(); i++) {
            // Check if end of string or is an operator
            if (i == input.size() || reservedTokens.contains(input[i]) ||
                operatorsCheck.contains(input[i])) {

                // If there's a word before the operator, add it to the result
                std::string tokenStr = trim(input.substr(tokenStart, i - tokenStart));
                if (!tokenStr.empty()) {
                    Token token(tokenStr);
                    // Detect unrecognizable tokens
                    if (token.type == TokenType::INVALID) {
                        throw ExpressionUnrecognizedToken(input, tokenStr);
                    } else if (token.type == TokenType::VALUE && !token.name.empty() && !unitIdx.contains(token.name)) {
                        throw ExpressionInvalidUnit(input, tokenStr, token.name);
                    }
                    result.push_back(token);
                }

                // Process the current operator
                if (i < input.size()) {
                    result.emplace_back(input.substr(i, 1));
                    tokenStart = i + 1;
                }
            }
        }
        return result;
    }

    std::vector<Token> toPostfix(const std::string& input, const std::vector<Token>& tokens) {
        std::vector<Token> pass1;
        std::vector<Token> pass2;
        std::stack<Token> operatorStack;

        // First pass: convert all functions and parentheses to postfix
        for (size_t i = 0; i < tokens.size(); i++) {
            const Token& token = tokens[i];
            switch (token.type) {
                case TokenType::FUNCTION:
                    // If is a function, push end function token to represent the end of argument list.
                    // Validate that it is succeeded by `(`.
                    pass1.emplace_back(TokenType::END_FUNC, "", 0.0f, false);
                    operatorStack.push(token);
                    if (i + 1 == tokens.size() || tokens[i + 1].type != TokenType::PARENTHESIS_LEFT) {
                        throw ExpressionSyntaxError(
                                input, "`(` expected after function `" + token.name + "`.");
                    }
                    break;
                case TokenType::PARENTHESIS_LEFT:
                    // If is `(`, push it to the operator stack.
                    // If previous  token is not function, add an `eval` function manually.
                    if (i == 0 || tokens[i - 1].type != TokenType::FUNCTION) {
                        pass1.emplace_back(TokenType::END_FUNC, "", 0.0f, false);
                        operatorStack.emplace(TokenType::FUNCTION, "eval", 0.0f, false);
                    }
                    // Push the `(`
                    operatorStack.push(token);
                    break;
                case TokenType::PARENTHESIS_RIGHT:
                    // If is `)`, pop the `(` on the operator stack.
                    // Validate that the top of the stack is `(`.
                    if (operatorStack.empty() || operatorStack.top().type != TokenType::PARENTHESIS_LEFT) {
                        throw ExpressionSyntaxError(input, "Token `)` is not paired with `(`.");
                    }
                    // Pop the `(`
                    operatorStack.pop();
                    // Push a separator before pushing the function
                    pass1.emplace_back(TokenType::SEPARATOR, "", 0.0f, false);
                    // Push the function
                    pass1.push_back(operatorStack.top());
                    operatorStack.pop();
                    break;
                default:
                    // If other tokens, let them fall through.
                    pass1.push_back(token);
                    break;
            }
        }

        // Verify that the operator stack is empty
        if (!operatorStack.empty()) {
            throw ExpressionSyntaxError(input, "Token `(` is not paired with `)`.");
        }

        // Second pass: convert all binary operators to postfix
        for (const Token& token: pass1) {
            switch (token.type) {
                case TokenType::VALUE:
                    // If is a value, let it fall through
                    pass2.push_back(token);
                    break;
                case TokenType::SEPARATOR:
                    // If is `,`, push all operators until reaches end function token, then push itself.
                    // Validate that such `(` can be found.
                    while (true) {
                        if (operatorStack.empty()) {
                            throw ExpressionSyntaxError(
                                    input, "Separator `,` used outside of function call.");
                        }
                        if (operatorStack.top().type == TokenType::END_FUNC) {
                            break;
                        }
                        // Keep moving operators from the stack to the result vector if not reached destination.
                        pass2.push_back(operatorStack.top());
                        operatorStack.pop();
                    }
                    // Push the operator itself to the stack
                    pass2.push_back(token);
                    break;
                case TokenType::OPERATOR:
                    // If is an operator:
                    while (!operatorStack.empty()) {
                        // Stop when the top of the stack is not an operator.
                        if (operatorStack.top().type != TokenType::OPERATOR) {
                            break;
                        }
                        // Stop when the top of the stack has smaller precedence than the new operator.
                        if (precedes(token.name[0], operatorStack.top().name[0])) {
                            break;
                        }
                        // Otherwise keep moving operators from the stack to the result vector.
                        pass2.push_back(operatorStack.top());
                        operatorStack.pop();
                    }
                    // Push the operator itself to the stack
                    operatorStack.push(token);
                    break;
                case TokenType::END_FUNC:
                    // If is an end function token, push to both operator stack and result
                    pass2.push_back(token);
                    operatorStack.push(token);
                    break;
                case TokenType::FUNCTION:
                    // If is a function, push itself and pop end function token from the stack.
                    // Function call must be preceded by a separator, which forces the top two of the stack to be
                    // the separator and the end function token.
                    pass2.push_back(token);
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
            if (operatorStack.top().type == TokenType::PARENTHESIS_LEFT) {
                throw ExpressionSyntaxError(input, "Token `(` is not paired with `)`.");
            }
            pass2.push_back(operatorStack.top());
            operatorStack.pop();
        }

        return pass2;
    }

    void validate(const std::string& input, const std::vector<Token>& tokens) {
        std::stack<Token> valueStack;

        for (const Token& token: tokens) {
            switch (token.type) {
                case TokenType::VALUE:
                    // If is value, push to stack.
                    valueStack.push(token);
                    break;
                case TokenType::SEPARATOR: {
                    // If is separator, validate that there is exactly one value since last end function token or
                    // separator.
                    if (valueStack.size() < 2) {
                        throw ExpressionSyntaxError(
                                input, "Separator `,` used outside of function call.");
                    }
                    // Retrieve last two values and verify
                    Token token2 = valueStack.top();
                    valueStack.pop();
                    Token token1 = valueStack.top();  // No need to pop this. Can use it to store result.
                    if (token2.type != TokenType::VALUE) {
                        throw ExpressionSyntaxError(
                                input, "Invalid function argument `" + token2.toString() + "`.");
                    }
                    // Push last value and new separator
                    if (token1.type == TokenType::SEPARATOR) {  // Remove the last separator
                        valueStack.top() = token2;
                        valueStack.push(token);
                    } else if (token1.type == TokenType::END_FUNC) {  // If this is the first separator, don't remove
                        valueStack.push(token2);
                        valueStack.push(token);
                    } else {
                        throw ExpressionSyntaxError(
                                input, "Expected separator `,` but get function argument `" + token1.toString() + "`.");
                    }
                    break;
                }
                case TokenType::OPERATOR: {
                    // If is operator, calculate the resulting unit.
                    if (valueStack.size() < 2) {
                        throw ExpressionSyntaxError(
                                input, "Binary operator `" + token.name + "` requires two operands.");
                    }
                    // Retrieve last two values
                    Token token2 = valueStack.top();
                    valueStack.pop();
                    Token token1 = valueStack.top();  // No need to pop this. Can use it to store result.
                    if (token1.type != TokenType::VALUE || token2.type != TokenType::VALUE) {
                        throw ExpressionSyntaxError(
                                input, "Binary operator `" + token.name + "` requires two operands.");
                    }
                    // Calculate and store result
                    bool result = operatorsCheck.at(token.name[0])
                            .second(input, token1.value.hasUnit, token2.value.hasUnit);
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
                    bool result = functionsCheck.at(token.name)(input, operands);
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
        if (valueStack.size() != 1 || valueStack.top().type != TokenType::VALUE) {
            throw ExpressionSyntaxError(input, "Invalid expression.");
        }
    }

    bool precedes(char op1, char op2) {
        return operatorsCheck.at(op1).first > operatorsCheck.at(op2).first;
    }

    std::string trim(const std::string& str) {
        const std::string whitespace = " \t\n\r\f\v";
        std::size_t start = str.find_first_not_of(whitespace);

        // If str consists entirely of whitespace
        if (start == std::string::npos) {
            return "";
        }

        std::size_t end = str.find_last_not_of(whitespace);
        return str.substr(start, end - start + 1);
    }
}

namespace corn {
    std::vector<Token>* duplicateTokens(std::vector<Token>* tokens) {
        if (tokens == nullptr) return nullptr;
        return new std::vector<Token>(*tokens);
    }

    void deleteTokens(std::vector<Token>* tokens) {
        delete tokens;
    }

    std::vector<Token>* expressionFromStringImpl(
            const std::string& input, const std::unordered_map<std::string, size_t>& unitIdx) {

        // Tokenization
        auto* tokens = new std::vector<Token>();
        *tokens = impl::expression::tokenize(input, unitIdx);
        // Infix to postfix
        *tokens = impl::expression::toPostfix(input, *tokens);
        // Validation
        impl::expression::validate(input, *tokens);
        return tokens;
    }

    float expressionCalcImpl(
            const std::vector<Token>& tokens, const std::unordered_map<std::string, size_t>& unitIdx,
            const std::vector<float>& valuesArray) {

        std::stack<Token> valueStack;

        for (const Token& token : tokens) {
            switch (token.type) {
                case impl::expression::TokenType::VALUE: {
                    // If is value, push to stack.
                    float newVal = token.value.hasUnit ?
                                   token.value.val * valuesArray[unitIdx.at(token.name)] : token.value.val;
                    valueStack.emplace(token.type, "", newVal, token.value.hasUnit);
                    break;
                }
                case impl::expression::TokenType::SEPARATOR: {
                    // If is separator, skip.
                    break;
                }
                case impl::expression::TokenType::OPERATOR: {
                    // If is operator, calculate the resulting unit.
                    // Retrieve last two values
                    Token token2 = valueStack.top();
                    valueStack.pop();
                    Token token1 = valueStack.top();  // No need to pop this. Can use it to store result.
                    // Calculate and store result
                    valueStack.top().value = impl::expression::operators.at(token.name[0])(token1.value, token2.value);
                    break;
                }
                case impl::expression::TokenType::END_FUNC:
                    // If is end function token, push to stack.
                    valueStack.push(token);
                    break;
                case impl::expression::TokenType::FUNCTION: {
                    // If is function, retrieve the list of arguments and calculate the resulting unit.
                    // Retrieve values until reaching end
                    std::vector<impl::expression::Value> operands;
                    while (valueStack.top().type != impl::expression::TokenType::END_FUNC) {
                        operands.push_back(valueStack.top().value);
                        valueStack.pop();
                    }
                    std::reverse(operands.begin(), operands.end());
                    // Calculate and store result
                    valueStack.top().type = impl::expression::TokenType::VALUE;
                    valueStack.top().value = impl::expression::functions.at(token.name)(operands);
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
