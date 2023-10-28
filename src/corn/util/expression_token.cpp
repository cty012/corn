#include "expression_helper.h"

namespace corn {
    Token::Token(impl::expression::TokenType type, std::string name, float val, bool hasUnit)
            : type(type), name(std::move(name)), value(val, hasUnit) {}

    Token::Token(const std::string& tokenStr) : value() {
        if (tokenStr == "(") {
            this->type = impl::expression::TokenType::PARENTHESIS_LEFT;
            this->name = "(";
        } else if (tokenStr == ")") {
            this->type = impl::expression::TokenType::PARENTHESIS_RIGHT;
            this->name = ")";
        } else if (tokenStr == ",") {
            this->type = impl::expression::TokenType::SEPARATOR;
            this->name = ",";
        } else if (tokenStr.size() == 1 && impl::expression::_operators.contains(tokenStr[0])) {
            this->type = impl::expression::TokenType::OPERATOR;
            this->name = tokenStr;
        } else if (impl::expression::_functions.contains(tokenStr)) {
            this->type = impl::expression::TokenType::FUNCTION;
            this->name = tokenStr;
        } else {
            try {
                std::size_t idx;
                this->value.val = std::stof(tokenStr, &idx);
                this->type = impl::expression::TokenType::VALUE;
                this->name = tokenStr.substr(idx);
                this->value.hasUnit = !this->name.empty();
            } catch (const std::exception&) {
                this->type = impl::expression::TokenType::INVALID;
            }
        }
    }

    std::string Token::toString() const {
        switch (this->type) {
            case impl::expression::TokenType::OPERATOR:
            case impl::expression::TokenType::FUNCTION:
                return this->name;
            case impl::expression::TokenType::END_FUNC:
                return "[END_FUNC]";
            case impl::expression::TokenType::PARENTHESIS_LEFT:
                return "(";
            case impl::expression::TokenType::PARENTHESIS_RIGHT:
                return ")";
            case impl::expression::TokenType::SEPARATOR:
                return ",";
            case impl::expression::TokenType::VALUE:
                return std::to_string(this->value.val) + this->name;
            case impl::expression::TokenType::INVALID:
                return "!";
        }
        return "";
    }
}
