#include <corn/util/exceptions.h>

namespace corn {
    InvalidTransform::InvalidTransform(const std::string& msg) noexcept {
        this->msg_ = "Invalid transformation matrix:\n" + msg + "\n";
    }

    const char* InvalidTransform::what() const noexcept {
        return this->msg_.c_str();
    }

    ResourceLoadFailed::ResourceLoadFailed(const std::string& msg) noexcept {
        this->msg_ = msg + "\n";
    }

    const char* ResourceLoadFailed::what() const noexcept {
        return this->msg_.c_str();
    }

    ExpressionParseFailed::ExpressionParseFailed(const std::string& input, const std::string& msg) noexcept {
        this->msg_ = "Error parsing expression from string:\n    Input string: " + input
                + "\n    Error: " + msg + "\n";
    }

    const char* ExpressionParseFailed::what() const noexcept {
        return this->msg_.c_str();
    }

    ExpressionParseFailed::ExpressionParseFailed() noexcept : msg_() {}

    ExpressionSyntaxError::ExpressionSyntaxError(const std::string& input, const std::string& msg) noexcept {
        this->msg_ = "Error parsing expression from string:\n    Input string: " + input
                + "\n    Syntax error: " + msg + "\n";
    }

    const char* ExpressionSyntaxError::what() const noexcept {
        return this->msg_.c_str();
    }

    ExpressionSyntaxError::ExpressionSyntaxError() noexcept : msg_() {}

    ExpressionUnrecognizedToken::ExpressionUnrecognizedToken(const std::string& input, const std::string& token) noexcept {
        this->msg_ = "Error parsing expression from string:\n    Input string: " + input
                + "\n    Unrecognized token: `" + token + "`.\n";
    }

    const char* ExpressionUnrecognizedToken::what() const noexcept {
        return this->msg_.c_str();
    }

    ExpressionUnrecognizedToken::ExpressionUnrecognizedToken() noexcept : msg_() {}

    ExpressionInvalidUnit::ExpressionInvalidUnit(
            const std::string& input, const std::string& token, const std::string& unit) noexcept {

        this->msg_ = "Error parsing expression from string:\n    Input string: " + input
                    + "\n    Invalid unit: unit `" + unit + "` in token `" + token + "` is invalid.\n";
    }

    const char* ExpressionInvalidUnit::what() const noexcept {
        return this->msg_.c_str();
    }

    ExpressionInvalidUnit::ExpressionInvalidUnit() noexcept : msg_() {}

    ExpressionUnitMismatch::ExpressionUnitMismatch(const std::string& input, const std::string& msg) noexcept {
        this->setMsg(input, msg);
    }

    ExpressionUnitMismatch::ExpressionUnitMismatch(
            const std::string& input, const std::string& binOp, bool a, bool b) noexcept {

        static const char* valueTypes[] = { "a number literal", "a number with unit" };
        this->setMsg(input, "Binary operator `" + binOp + "` cannot be applied between "
                + valueTypes[a] + "and" + valueTypes[b] + ".");
    }

    ExpressionUnitMismatch::ExpressionUnitMismatch(
            const std::string& input, const std::string& binOp, const std::vector<bool>& operands) noexcept {

        static const char* valueTypes[] = { "a number literal", "a number with unit" };
        std::string temp;
        for (size_t i = 0; i < operands.size(); i++) {
            if (i) temp += ", ";
            temp += valueTypes[operands[i]];
        }
        this->setMsg(input, "Binary operator `" + binOp + "` cannot take the parameter list (" + temp + ").");
    }

    const char* ExpressionUnitMismatch::what() const noexcept {
        return this->msg_.c_str();
    }

    ExpressionUnitMismatch::ExpressionUnitMismatch() noexcept : msg_() {}

    void ExpressionUnitMismatch::setMsg(const std::string& input, const std::string& msg) noexcept {
        this->msg_ = "Error parsing expression from string:\n    Input string: " + input
                     + "\n    Unit mismatch: " + msg + "\n";
    }
}
