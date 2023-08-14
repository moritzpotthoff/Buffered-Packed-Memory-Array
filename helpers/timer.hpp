#pragma once
#include <chrono>

namespace Helpers {

    /**
     * Simple Timer used for the evaluation.
     */
    class Timer {
    public:
        using clockType = std::chrono::high_resolution_clock;
        Timer() : start(clockType::now()) {}

        inline void restart() noexcept {
            start = clockType::now();
        }

        inline size_t getMilliseconds() {
            std::chrono::time_point<clockType, std::chrono::nanoseconds> end = clockType::now();
            return (size_t)std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        }

        inline size_t getMicroseconds() {
            std::chrono::time_point<clockType, std::chrono::nanoseconds> end = clockType::now();
            return (size_t)std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        }

        inline size_t getNanoseconds() {
            std::chrono::time_point<clockType, std::chrono::nanoseconds> end = clockType::now();
            return (size_t)std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
        }

        inline auto getTime() {
            std::chrono::time_point<clockType, std::chrono::nanoseconds> end = clockType::now();
            return end - start;
        }

    private:
        std::chrono::time_point<clockType, std::chrono::nanoseconds> start;
    };
}