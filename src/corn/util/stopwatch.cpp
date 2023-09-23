#include <corn/util/stopwatch.h>

namespace corn {
    Stopwatch::Stopwatch() : running(false), elapsedTime(0) {}

    void Stopwatch::play() {
        std::lock_guard<std::mutex> lock(mutex);
        if (this->running) return;  // No effect if stopwatch already running
        this->running = true;
        this->startTime = std::chrono::high_resolution_clock::now();
    }

    void Stopwatch::pause() {
        std::lock_guard<std::mutex> lock(mutex);
        if (!this->running) return;  // No effect if stopwatch already paused
        this->running = false;
        auto endTime = std::chrono::high_resolution_clock::now();
        this->elapsedTime += std::chrono::duration_cast<std::chrono::milliseconds>(
                endTime - this->startTime).count();
    }

    void Stopwatch::clear() {
        std::lock_guard<std::mutex> lock(mutex);
        this->running = false;
        this->elapsedTime = 0;
    }

    unsigned long long int Stopwatch::millis() const {
        std::lock_guard<std::mutex> lock(mutex);
        if (this->running) {
            auto currentTime = std::chrono::high_resolution_clock::now();
            return this->elapsedTime + std::chrono::duration_cast<std::chrono::milliseconds>(
                    currentTime - this->startTime).count();
        } else {
            return this->elapsedTime;
        }
    }

    bool Stopwatch::isRunning() const {
        return this->running;
    }
}
