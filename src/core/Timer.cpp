#include "Timer.h"

Timer::Timer() : m_start(std::chrono::high_resolution_clock::now()) {}

float Timer::restart()
{
    auto now = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<float>(now - m_start).count();
    m_start = now;
    return duration;
}

float Timer::elapsed() const
{
    auto now = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<float>(now - m_start).count();
}
