/**
 * @file stopwatch.h
 * @brief Defines a stopwatch utility with simple functions.
 */

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
        Stopwatch();

        /// @brief Start or resume the stopwatch.
        void play();

        /// @brief Pause the stopwatch.
        void pause();

        /// @brief Reset to initial state.
        void clear();

        /// @return The elapsed time in milliseconds.
        float millis() const;

        /// @return Whether the stopwatch is running.
        bool isRunning() const;

    private:
        bool running_;
        std::chrono::high_resolution_clock::time_point startTime_;
        float elapsedTime_;
        mutable std::mutex mutex_;
    };
}
