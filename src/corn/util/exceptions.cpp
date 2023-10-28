#include <utility>
#include <corn/util/exceptions.h>

namespace corn {
    ResourceLoadFailed::ResourceLoadFailed(const std::string& msg) {
        this->msg = msg + "\n";
    }

    const char* ResourceLoadFailed::what() const noexcept {
        return this->msg.c_str();
    }

    ExpressionParseFailed::ExpressionParseFailed(const std::string& msg) {
        this->msg = "Error parsing expression from string:\n    " + msg + "\n";
    }

    const char* ExpressionParseFailed::what() const noexcept {
        return this->msg.c_str();
    }
}
