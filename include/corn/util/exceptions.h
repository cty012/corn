#pragma once

/**
 * Defines all custom exceptions
 */

#include <stdexcept>
#include <string>

namespace corn {
    /**
     * Currently empty
     */
    class ResourceLoadFailed : public std::exception {
    public:
        explicit ResourceLoadFailed(std::string msg);
        [[nodiscard]] const char* what() const noexcept override;
    private:
        std::string msg;
    };
}
