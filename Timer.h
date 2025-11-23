// include/Timer.h
#ifndef TIMER_H
#define TIMER_H

#include <chrono>

class Timer {
private:
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::high_resolution_clock::time_point endTime;
    bool isRunning = false;

public:
    void start();
    void stop();
    double elapsedMilliseconds() const;
    double elapsedMicroseconds() const;
};

#endif // TIMER_H