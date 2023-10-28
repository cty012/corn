#include "expression_impl.h"

namespace corn::impl::expression {
    const std::unordered_set<char> reservedTokens = {'(', ')', ','};

    const std::unordered_map<char, std::pair<int, std::function<bool(bool, bool)>>> _operators = {
            {
                '+', {0, [](bool a, bool b) -> bool {
                    if (a != b)
                        throw ExpressionParseFailed("Cannot add number literals and numbers with unit.");
                    return a;
                }}
            },
            {
                '-', {0, [](bool a, bool b) -> bool {
                    if (a != b)
                        throw ExpressionParseFailed("Cannot subtract number literals and numbers with unit.");
                    return a;
                }}
            },
            {
                '*', {1, [](bool a, bool b) -> bool {
                    if (a && b)
                        throw ExpressionParseFailed("Cannot multiply two numbers with unit.");
                    return a || b;
                }}
            },
            {
                '/', {1, [](bool a, bool b) -> bool {
                    if (!a && b)
                        throw ExpressionParseFailed("Cannot divide number literals by numbers with unit.");
                    return a && !b;
                }}
            },
    };

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

    const std::unordered_map<std::string, const std::function<bool(const std::vector<bool>&)>> _functions = {
            {
                "eval", [](const std::vector<bool>& operands) -> bool {
                    if (operands.empty())
                        throw ExpressionParseFailed("Empty parentheses.");
                    else if (operands.size() > 1)
                        throw ExpressionParseFailed("Separator `,` used outside of function call.");
                    return operands[0];
                }
            },
            {
                "min", [](const std::vector<bool>& operands) -> bool {
                    if (operands.empty())
                        throw ExpressionParseFailed("Function `min` takes at least one operand.");
                    size_t countHasUnit = 0;
                    for (bool operand : operands) {
                        countHasUnit += operand;
                    }
                    if (countHasUnit != 0 && countHasUnit != operands.size())
                        throw ExpressionParseFailed("Cannot take minimum of number literals and numbers with units.");
                    return operands[0];
                }
            },
            {
                "max", [](const std::vector<bool>& operands) -> bool {
                    if (operands.empty())
                        throw ExpressionParseFailed("Function `max` takes at least one operand.");
                    size_t countHasUnit = 0;
                    for (bool operand : operands) {
                        countHasUnit += operand;
                    }
                    if (countHasUnit != 0 && countHasUnit != operands.size())
                        throw ExpressionParseFailed("Cannot take maximum of number literals and numbers with units.");
                    return operands[0];
                }
            },
    };

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
}
