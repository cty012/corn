#include <utility>
#include <corn/util/exceptions.h>

namespace corn {
    ResourceLoadFailed::ResourceLoadFailed(const std::string& msg) {
        this->msg = msg + "\n";
    }

    const char* ResourceLoadFailed::what() const noexcept {
        return this->msg.c_str();
    }

    RelValParseFailed::RelValParseFailed(const std::string& msg) {
        this->msg = "Error parsing relative value from string:\n    " + msg + "\n";
    }

    const char* RelValParseFailed::what() const noexcept {
        return this->msg.c_str();
    }
}
