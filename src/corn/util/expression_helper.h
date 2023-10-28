#include <functional>
#include <unordered_set>
#include <corn/util/expression.h>

namespace corn::impl::expression {
    /**
     * @class impl::expression::TokenType
     * @brief Types of tokens.
     */
    enum class TokenType {
        OPERATOR, FUNCTION, END_FUNC, PARENTHESIS_LEFT, PARENTHESIS_RIGHT, SEPARATOR, VALUE, INVALID,
    };

    /**
     * @class impl::expression::Value
     * @brief Simple struct for holding a floating point number with or without unit.
     */
    struct Value {
        float val;
        bool hasUnit;
    };

    /// @brief Reserved tokens with special meanings.
    extern const std::unordered_set<char> reservedTokens;

    /**
     * @brief Binary operators syntax checker: ? <OP> ?
     *
     * The expression is only validated once when parsing the expression string. The validation functions throws an
     * error if the number and type of the arguments is invalid, and returns the appropriate type.
     *
     * The type of the variable is represented by a bool value. True means the value has a unit, and false means it is
     * a number literal.
     */
    extern const std::unordered_map<char, std::pair<int, std::function<bool(bool, bool)>>> _operators;

    /**
     * @brief Binary operators: ? <OP> ?
     *
     * The int value represent the precedence of the operator. Higher precedence operators are calculated before
     * lower precedence operators.
     */
    extern const std::unordered_map<char, const std::function<Value(Value, Value)>> operators;

    /**
     * @brief Function syntax checker: <FUNC>(?, ?, ..., ?)
     *
     * The expression is only validated once when parsing the expression string. The validation functions throws an
     * error if the number and type of the arguments is invalid, and returns the appropriate type.
     *
     * The type of the variable is represented by a bool value. True means the value has a unit, and false means it is
     * a number literal.
     */
    extern const std::unordered_map<std::string, const std::function<bool(const std::vector<bool>&)>> _functions;

    /**
     * @brief Functions: <FUNC>(?, ?, ..., ?)
     *
     * The argument list will be passed in as a vector of Values. Functions need to verify the number of arguments
     * themselves.
     */
    extern const std::unordered_map<std::string, const std::function<Value(const std::vector<Value>&)>> functions;

    /**
     * @brief Breaks the input string into tokens.
     * @param expression The expression string to be calculated.
     * @param units Unordered map of all valid units to their index.
     * @return A vector of tokens.
     */
    std::vector<Token> tokenize(const std::string& expression, const std::unordered_map<std::string, size_t>& unitIdx);

    /**
     * @param tokens A vector of tokens in infix notation.
     * @return The same set of tokens but in postfix notation.
     *
     * Uses a variation of Shunting-yard algorithm to convert from infix to postfix.
     */
    std::vector<Token> toPostfix(const std::vector<Token>& tokens);

    /**
     * @brief Validates whether the parsed expression has valid syntax.
     * @param tokens The vector of token to be validated.
     */
    void validate(const std::vector<Token>& tokens);

    /**
     * @param op1 First operator.
     * @param op2 Second operator.
     * @return Whether op1 precedes op2.
     *
     * This function assumes that both operators are valid.
     */
    bool precedes(char op1, char op2);

    /// @brief Utility function. Removes spaces and new lines from the begin and end of a string.
    std::string trim(const std::string& str);
}

namespace corn {
    /**
     * @class Token
     * @brief Stores all information about the token.
     *
     * If token is not a VALUE, the string representation will be stored in name. If it is a VALUE, the numerical
     * part will be stored in value.val and the unit will be stored in name.
     */
    struct Token {
        /// @brief Default constructor.
        Token(impl::expression::TokenType type, std::string name, float val, bool hasUnit);

        /**
         * @param tokenStr String representation of the token.
         *
         * Warning: having type VALUE does not imply that the unit exists. It only means that a floating point number
         * can be parsed from it (and the remaining part becomes the unit). If there is no remaining part, the number
         * is interpreted as a number literal.
         */
        explicit Token(const std::string& tokenStr);

        /// @return String representation of the Token
        [[nodiscard]] std::string toString() const;

        impl::expression::TokenType type;
        std::string name;
        impl::expression::Value value;
    };
}
