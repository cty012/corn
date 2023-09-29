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
        double millis() const;

        /// @return Whether the stopwatch is running.
        bool isRunning() const;

    private:
        mutable std::mutex mutex;
        bool running;
        double elapsedTime;
        std::chrono::high_resolution_clock::time_point startTime;
    };
}
