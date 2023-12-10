#include <corn/util/stopwatch.h>

namespace corn {
    Stopwatch::Stopwatch() noexcept : running_(false), startTime_(), elapsedTime_(0.0f), mutex_() {}

    void Stopwatch::play() noexcept {
        std::lock_guard<std::mutex> lock(mutex_);
        if (this->running_) return;  // No effect if stopwatch already running
        this->running_ = true;
        this->startTime_ = std::chrono::high_resolution_clock::now();
    }

    void Stopwatch::pause() noexcept {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!this->running_) return;  // No effect if stopwatch already paused
        this->running_ = false;
        auto endTime = std::chrono::high_resolution_clock::now();
        this->elapsedTime_ += std::chrono::duration<float, std::milli>(
                endTime - this->startTime_).count();
    }

    void Stopwatch::clear() noexcept {
        std::lock_guard<std::mutex> lock(mutex_);
        this->running_ = false;
        this->elapsedTime_ = 0;
    }

    float Stopwatch::millis() const noexcept {
        std::lock_guard<std::mutex> lock(mutex_);
        if (this->running_) {
            auto currentTime = std::chrono::high_resolution_clock::now();
            return this->elapsedTime_ + std::chrono::duration<float, std::milli>(
                    currentTime - this->startTime_).count();
        } else {
            return this->elapsedTime_;
        }
    }

    bool Stopwatch::isRunning() const noexcept {
        return this->running_;
    }
}
