#pragma once

/**
 * Defines all custom exceptions
 */

#include <stdexcept>
#include <string>

namespace corn {
    class ResourceLoadFailed : public std::exception {
    public:
        explicit ResourceLoadFailed(const std::string& msg);
        [[nodiscard]] const char* what() const noexcept override;
    private:
        std::string msg;
    };

    class ExpressionParseFailed : public std::exception {
    public:
        explicit ExpressionParseFailed(const std::string& msg);
        [[nodiscard]] const char* what() const noexcept override;
    private:
        std::string msg;
    };
}
