#include <utility>
#include <corn/util/exceptions.h>

namespace corn {
    ResourceLoadFailed::ResourceLoadFailed(std::string msg) : msg(std::move(msg)) {}

    const char* ResourceLoadFailed::what() const noexcept {
        return this->msg.c_str();
    }
}
