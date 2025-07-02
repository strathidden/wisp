#pragma once
#include <string>
#include <format>
#include <iostream>

#define LOG_CRITICAL(...) Logger::log(LogLevel::Critical, __VA_ARGS__)
#define LOG_ERROR(...) Logger::log(LogLevel::Error, __VA_ARGS__)
#define LOG_WARNING(...) Logger::log(LogLevel::Warning, __VA_ARGS__)
#define LOG_INFO(...) Logger::log(LogLevel::Info, __VA_ARGS__)
#define LOG_DEBUG(...) Logger::log(LogLevel::Debug, __VA_ARGS__)

enum class LogLevel
{
    Debug,
    Info,
    Warning,
    Error,
    Critical
};

class Logger
{
public:
    template <typename... Args>
    static void log(LogLevel level, const std::string& fmt, Args&&... args)
    {
        std::string message = std::vformat(fmt, std::make_format_args(args...));
        log(level, message);
    }

    static void log(LogLevel level, const std::string& message)
    {
        const char* levelStr = "";
        switch (level)
        {
            case LogLevel::Debug: levelStr = "DEBUG"; break;
            case LogLevel::Info: levelStr = "INFO"; break;
            case LogLevel::Warning: levelStr = "WARNING"; break;
            case LogLevel::Error: levelStr = "ERROR"; break;
            case LogLevel::Critical: levelStr = "CRITICAL"; break;

        }
        std::cerr << "[" << levelStr << "]" << message << "\n";
    }
};
