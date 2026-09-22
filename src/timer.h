#ifndef TIMER_H
#define TIMER_H
#include <chrono>
#include <cstdint>
class Timer {
private:
    std::chrono::time_point<std::chrono::steady_clock> start_time;

public:
    // Timer::Nanos
    using Nanos = std::chrono::nanoseconds;
    // Timer::Micros
    using Micros = std::chrono::microseconds;
    // Timer::Millis
    using Millis = std::chrono::milliseconds;
    // Timer::Seconds
    using Seconds = std::chrono::seconds;
    // Timer::Minutes
    using Minutes = std::chrono::minutes;
    // Timer::Hours
    using Hours = std::chrono::hours;

    // constructor - start timer
    Timer() : start_time(std::chrono::steady_clock::now()) {}

    // restarts timer
    void restart() { start_time = std::chrono::steady_clock::now(); }

    template <typename T>
    uint64_t click() {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<T>(now - start_time)
                           .count();  // count to convert from type duration to int
        start_time = now;
        return elapsed;
    }

    template <typename T>
    uint64_t glance() const {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<T>(now - start_time)
                           .count();  // count to convert from type duration to int
        return elapsed;
    }
};
#endif  // TIMER_H
