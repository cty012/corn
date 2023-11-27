/**
 * @file exceptions.h
 * @brief Defines all custom exceptions used by corn.
 */

#pragma once

#include <stdexcept>
#include <string>
#include <vector>

namespace corn {
    class ResourceLoadFailed : public std::exception {
    public:
        explicit ResourceLoadFailed(const std::string& msg);
        [[nodiscard]] const char* what() const noexcept override;

    private:
        std::string msg_;
    };

    class ExpressionParseFailed : public std::exception {
    public:
        ExpressionParseFailed(const std::string& input, const std::string& msg);
        [[nodiscard]] const char* what() const noexcept override;

    protected:
        ExpressionParseFailed();

    private:
        std::string msg_;
    };

    class ExpressionSyntaxError : public ExpressionParseFailed {
    public:
        ExpressionSyntaxError(const std::string& input, const std::string& msg);
        [[nodiscard]] const char* what() const noexcept override;

    protected:
        ExpressionSyntaxError();

    private:
        std::string msg_;
    };

    class ExpressionUnrecognizedToken : public ExpressionSyntaxError {
    public:
        ExpressionUnrecognizedToken(const std::string& input, const std::string& token);
        [[nodiscard]] const char* what() const noexcept override;

    protected:
        ExpressionUnrecognizedToken();

    private:
        std::string msg_;
    };

    class ExpressionInvalidUnit : public ExpressionSyntaxError {
    public:
        ExpressionInvalidUnit(const std::string& input, const std::string& token, const std::string& unit);
        [[nodiscard]] const char* what() const noexcept override;

    protected:
        ExpressionInvalidUnit();

    private:
        std::string msg_;
    };

    class ExpressionUnitMismatch : public ExpressionParseFailed {
    public:
        ExpressionUnitMismatch(const std::string& input, const std::string& msg);
        ExpressionUnitMismatch(const std::string& input, const std::string& binOp, bool a, bool b);
        ExpressionUnitMismatch(const std::string& input, const std::string& binOp, const std::vector<bool>& operands);
        [[nodiscard]] const char* what() const noexcept override;

    protected:
        ExpressionUnitMismatch();

    private:
        void setMsg(const std::string& input, const std::string& msg);
        std::string msg_;
    };
}
