#pragma once
#include <chrono>

class Timer
{
public:
    Timer();
    float restart();
    float elapsed() const;

private:
    std::chrono::time_point<std::chrono::high_resolution_clock> m_start;
};
