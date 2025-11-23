// src/Timer.cpp
#include "Timer.h"

void Timer::start() {
    startTime = std::chrono::high_resolution_clock::now();
    isRunning = true;
}

void Timer::stop() {
    endTime = std::chrono::high_resolution_clock::now();
    isRunning = false;
}

double Timer::elapsedMilliseconds() const {
    auto end = isRunning ? std::chrono::high_resolution_clock::now() : endTime;
    return std::chrono::duration<double, std::milli>(end - startTime).count();
}

double Timer::elapsedMicroseconds() const {
    auto end = isRunning ? std::chrono::high_resolution_clock::now() : endTime;
    return std::chrono::duration<double, std::micro>(end - startTime).count();
}