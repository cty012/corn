#pragma once

#include <chrono>
#include <mutex>

namespace corn {
    /**
     * @class Stopwatch
     * @brief A stopwatch with play, pause, clear, and setActive functions.
     */
    class Stopwatch {
    public:
        /// Constructor
        Stopwatch() noexcept;

        /// @brief Start or resume the stopwatch.
        void play() noexcept;

        /// @brief Pause the stopwatch.
        void pause() noexcept;

        /// @brief Reset to initial state.
        void clear() noexcept;

        /// @return The elapsed time in milliseconds.
        float millis() const noexcept;

        /// @return Whether the stopwatch is running.
        bool isRunning() const noexcept;

    private:
        /// @brief Stores whether the stopwatch is running.
        bool running_;

        /// @brief Time when the stopwatch started timing.
        std::chrono::high_resolution_clock::time_point startTime_;

        /// @brief Time recorded by the stopwatch BEFORE it started timing last time.
        float elapsedTime_;

        /// @brief Mutex lock for multithreading.
        mutable std::mutex mutex_;
    };
}
