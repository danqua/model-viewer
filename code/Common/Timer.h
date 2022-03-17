#pragma once
#include <chrono>
#include <string>
#include <stdio.h>

class Timer
{
public:
    Timer(const std::string& caption) :
        m_Caption(caption)
    {
        m_Start = std::chrono::system_clock::now();
    }

    ~Timer()
    {
        auto end = std::chrono::system_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - m_Start);
        printf("[%s] %lld ms\n", m_Caption.c_str(), static_cast<long long int>(elapsed.count()));
    }
private:
    Timer() = delete;

private:
    std::string m_Caption;
    std::chrono::system_clock::time_point m_Start;
};