#pragma once

#include <stdexcept>
#include <string>
#include <vector>

namespace corn {
    /**
     * @class ResourceLoadFailed
     * @brief Throws when failed to load a resource from the disk (e.g. image, font).
     */
    class ResourceLoadFailed : public std::exception {
    public:
        explicit ResourceLoadFailed(const std::string& msg) noexcept;
        [[nodiscard]] const char* what() const noexcept override;

    private:
        std::string msg_;
    };

    /**
     * @class ExpressionParseFailed
     * @brief Base class of all errors related to parsing expressions for the Expression class.
     */
    class ExpressionParseFailed : public std::exception {
    public:
        ExpressionParseFailed(const std::string& input, const std::string& msg) noexcept;
        [[nodiscard]] const char* what() const noexcept override;

    protected:
        ExpressionParseFailed() noexcept;

    private:
        std::string msg_;
    };

    /**
     * @class ExpressionSyntaxError
     * @brief Throws when the expression to be parsed has a syntax error.
     */
    class ExpressionSyntaxError : public ExpressionParseFailed {
    public:
        ExpressionSyntaxError(const std::string& input, const std::string& msg) noexcept;
        [[nodiscard]] const char* what() const noexcept override;

    protected:
        ExpressionSyntaxError() noexcept;

    private:
        std::string msg_;
    };

    /**
     * @class ExpressionUnrecognizedToken
     * @brief Throws when the expression contains a token that is unrecognizable.
     */
    class ExpressionUnrecognizedToken : public ExpressionSyntaxError {
    public:
        ExpressionUnrecognizedToken(const std::string& input, const std::string& token) noexcept;
        [[nodiscard]] const char* what() const noexcept override;

    protected:
        ExpressionUnrecognizedToken() noexcept;

    private:
        std::string msg_;
    };

    /**
     * @class ExpressionInvalidUnit
     * @brief Throws when the expression contains a numerical value with a unrecognizable unit.
     */
    class ExpressionInvalidUnit : public ExpressionSyntaxError {
    public:
        ExpressionInvalidUnit(const std::string& input, const std::string& token, const std::string& unit) noexcept;
        [[nodiscard]] const char* what() const noexcept override;

    protected:
        ExpressionInvalidUnit() noexcept;

    private:
        std::string msg_;
    };

    /**
     * @class ExpressionUnitMismatch
     * @brief Throws when the units of the operands is invalid for the given operator/function.
     */
    class ExpressionUnitMismatch : public ExpressionParseFailed {
    public:
        ExpressionUnitMismatch(const std::string& input, const std::string& msg) noexcept;
        ExpressionUnitMismatch(const std::string& input, const std::string& binOp, bool a, bool b) noexcept;
        ExpressionUnitMismatch(const std::string& input, const std::string& binOp, const std::vector<bool>& operands) noexcept;
        [[nodiscard]] const char* what() const noexcept override;

    protected:
        ExpressionUnitMismatch() noexcept;

    private:
        void setMsg(const std::string& input, const std::string& msg) noexcept;
        std::string msg_;
    };
}
