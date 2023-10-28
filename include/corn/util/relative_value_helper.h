#pragma once

#include <string>
#include <unordered_set>
#include <corn/util/exceptions.h>

namespace corn::impl::rel_val {
    // Utility function
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

    /**
     * @class impl::rel_val::TokenType
     * @brief Types of tokens.
     */
    enum class TokenType {
        OPERATOR, FUNCTION, END_FUNC, PARENTHESIS_LEFT, PARENTHESIS_RIGHT, SEPARATOR, VALUE, INVALID,
    };

    /**
     * @class impl::rel_val::Value
     * @brief Simple struct for holding a floating point number with or without unit.
     */
    struct Value {
        float val;
        bool hasUnit;
    };

    /**
     * @class impl::rel_val::Token
     * @brief Stores all information about the token.
     *
     * If token is not a VALUE, the string representation will be stored in name. If it is a VALUE, the numerical
     * part will be stored in value.val and the unit will be stored in name.
     */
    struct Token {
        Token(TokenType type, std::string name, float val, bool hasUnit);
        explicit Token(const std::string& tokenStr);
        [[nodiscard]] std::string toString() const;
        TokenType type;
        std::string name;
        Value value;
    };

    /// @brief Reserved tokens with special meanings.
    const std::unordered_set<char> reservedTokens = {'(', ')', ','};

    /**
     * @brief Binary operators syntax checker: ? <OP> ?
     *
     * The expression is only validated once when parsing the expression string. The validation functions throws an
     * error if the number and type of the arguments is invalid, and returns the appropriate type.
     *
     * The type of the variable is represented by a bool value. True means the value has a unit, and false means it is
     * a number literal.
     */
    const std::unordered_map<char, std::pair<int, std::function<bool(bool, bool)>>> _operators = {
            {
                '+', {0, [](bool a, bool b) -> bool {
                    if (a != b)
                        throw RelValParseFailed("Cannot add number literals and numbers with unit.");
                    return a;
                }}
            },
            {
                '-', {0, [](bool a, bool b) -> bool {
                    if (a != b)
                        throw RelValParseFailed("Cannot subtract number literals and numbers with unit.");
                    return a;
                }}
            },
            {
                '*', {1, [](bool a, bool b) -> bool {
                    if (a && b)
                        throw RelValParseFailed("Cannot multiply two numbers with unit.");
                    return a || b;
                }}
            },
            {
                '/', {1, [](bool a, bool b) -> bool {
                    if (!a && b)
                        throw RelValParseFailed("Cannot divide number literals by numbers with unit.");
                    return a && !b;
                }}
            },
    };

    /**
     * @brief Binary operators: ? <OP> ?
     *
     * The int value represent the precedence of the operator. Higher precedence operators are calculated before
     * lower precedence operators.
     */
    const std::unordered_map<char, const std::function<Value(Value, Value)>> operators = {
            {
                '+', [](Value a, Value b) -> Value {
                    return Value(a.val + b.val, a.hasUnit);
                }
            },
            {
                '-', [](Value a, Value b) -> Value {
                    return Value(a.val - b.val, a.hasUnit);
                }
            },
            {
                '*', [](Value a, Value b) -> Value {
                    return Value(a.val * b.val, a.hasUnit || b.hasUnit);
                }
            },
            {
                '/', [](Value a, Value b) -> Value {
                    return Value(a.val / b.val, a.hasUnit && !b.hasUnit);
                }
            },
    };

    /**
     * @brief Function syntax checker: <FUNC>(?, ?, ..., ?)
     *
     * The expression is only validated once when parsing the expression string. The validation functions throws an
     * error if the number and type of the arguments is invalid, and returns the appropriate type.
     *
     * The type of the variable is represented by a bool value. True means the value has a unit, and false means it is
     * a number literal.
     */
    const std::unordered_map<std::string, const std::function<bool(const std::vector<bool>&)>> _functions = {
            {
                "eval", [](const std::vector<bool>& operands) -> bool {
                    if (operands.empty())
                        throw RelValParseFailed("Empty parentheses.");
                    else if (operands.size() > 1)
                        throw RelValParseFailed("Separator `,` used outside of function call.");
                    return operands[0];
                }
            },
            {
                "min", [](const std::vector<bool>& operands) -> bool {
                    if (operands.empty())
                        throw RelValParseFailed("Function `min` takes at least one operand.");
                    size_t countHasUnit = 0;
                    for (bool operand : operands) {
                        countHasUnit += operand;
                    }
                    if (countHasUnit != 0 && countHasUnit != operands.size())
                        throw RelValParseFailed("Cannot take minimum of number literals and numbers with units.");
                    return operands[0];
                }
            },
            {
                "max", [](const std::vector<bool>& operands) -> bool {
                    if (operands.empty())
                        throw RelValParseFailed("Function `max` takes at least one operand.");
                    size_t countHasUnit = 0;
                    for (bool operand : operands) {
                        countHasUnit += operand;
                    }
                    if (countHasUnit != 0 && countHasUnit != operands.size())
                        throw RelValParseFailed("Cannot take maximum of number literals and numbers with units.");
                    return operands[0];
                }
            },
    };

    /**
     * @brief Functions: <FUNC>(?, ?, ..., ?)
     *
     * The argument list will be passed in as a vector of Values. Functions need to verify the number of arguments
     * themselves.
     */
    const std::unordered_map<std::string, const std::function<Value(const std::vector<Value>&)>> functions = {
            {
                "eval", [](const std::vector<Value>& operands) -> Value {
                    return operands[0];
                }
            },
            {
                "min", [](const std::vector<Value>& operands) -> Value {
                    float minVal = operands[0].val;
                    for (const Value& operand : operands) {
                        minVal = std::min(minVal, operand.val);
                    }
                    return {minVal, operands[0].hasUnit};
                }
            },
            {
                "max", [](const std::vector<Value>& operands) -> Value {
                    float maxVal = operands[0].val;
                    for (const Value& operand : operands) {
                        maxVal = std::max(maxVal, operand.val);
                    }
                    return {maxVal, operands[0].hasUnit};
                }
            },
    };

    /// @brief Default constructor.
    Token::Token(TokenType type, std::string name, float val, bool hasUnit)
            : type(type), name(std::move(name)), value(val, hasUnit) {}

    /**
     * @param tokenStr String representation of the token.
     * @return The token type.
     *
     * Having type VALUE does not imply that the unit exists.
     */
    Token::Token(const std::string& tokenStr): value() {
        if (tokenStr == "(") {
            this->type = TokenType::PARENTHESIS_LEFT;
            this->name = "(";
        } else if (tokenStr == ")") {
            this->type = TokenType::PARENTHESIS_RIGHT;
            this->name = ")";
        } else if (tokenStr == ",") {
            this->type = TokenType::SEPARATOR;
            this->name = ",";
        } else if (tokenStr.size() == 1 && _operators.contains(tokenStr[0])) {
            this->type = TokenType::OPERATOR;
            this->name = tokenStr;
        } else if (_functions.contains(tokenStr)) {
            this->type = TokenType::FUNCTION;
            this->name = tokenStr;
        } else {
            try {
                std::size_t idx;
                this->value.val = std::stof(tokenStr, &idx);
                this->type = TokenType::VALUE;
                this->name = tokenStr.substr(idx);
                this->value.hasUnit = !this->name.empty();
            } catch (const std::exception&) {
                this->type = TokenType::INVALID;
            }
        }
    }

    /// @return String representation of the Token
    std::string Token::toString() const {
        switch (this->type) {
            case TokenType::OPERATOR:
            case TokenType::FUNCTION:
                return this->name;
            case TokenType::END_FUNC:
                return "[END_FUNC]";
            case TokenType::PARENTHESIS_LEFT:
                return "(";
            case TokenType::PARENTHESIS_RIGHT:
                return ")";
            case TokenType::SEPARATOR:
                return ",";
            case TokenType::VALUE:
                return std::to_string(this->value.val) + this->name;
            case TokenType::INVALID:
                return "!";
        }
        return "";
    }

    /**
     * @param op1 First operator.
     * @param op2 Second operator.
     * @return Whether op1 precedes op2.
     *
     * This function assumes that all operators are valid.
     */
    bool precedes(char op1, char op2) {
        return _operators.at(op1).first > _operators.at(op2).first;
    }
}
