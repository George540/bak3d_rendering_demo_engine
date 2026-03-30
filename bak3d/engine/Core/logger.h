/* ===========================================================================
The MIT License (MIT)

Copyright (c) 2022-2026 George Mavroeidis - GeoGraphics

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
=========================================================================== */

#pragma once

#include <chrono>
#include <cstdio>

namespace log_helper
{
    constexpr const char* extract_filename(const char* path)
    {
        const char* last = path;
        for (const char* p = path; *p != '\0'; ++p)
        {
            if (*p == '/' || *p == '\\')
            {
                last = p + 1;
            }
        }
        return last;
    }
}

#define LOG_FILENAME (log_helper::extract_filename(__FILE__))

enum class LogLevel
{
    LOG,
    WARNING,
    ERROR
};

/*
 * Simple Logger class for logging different levels of logging severity (Log, Warning, Error).
 */
class Logger
{
public:
    static void initialize();
    static void shutdown();
    template<typename... Args>
    static void log(const char* file, int line, LogLevel level, const char* fmt, Args... args)
    {
        const char* colour = "\033[0m";
        const char* tag = "LOG";
        switch (level)
        {
            case LogLevel::LOG:     colour = "\033[37m";   tag = "LOG";  break;
            case LogLevel::WARNING: colour = "\033[33m";   tag = "WARN"; break;
            case LogLevel::ERROR:   colour = "\033[1;31m"; tag = "ERR";  break;
        }

        std::string elapsed_time_formatted = get_elapsed_time_formatted();
        std::fprintf(stderr, "%s[%s][%s] %s:%d - ", colour, elapsed_time_formatted.c_str(), tag, file, line);
 
        // Print user message
        std::fprintf(stderr, fmt, args...);
        std::fprintf(stderr, "\033[0m\n");
    }
private:
    static std::chrono::steady_clock::time_point start_time;
    static std::string get_elapsed_time_formatted();
};

#define B3D_LOG_INFO(fmt, ...) \
    Logger::log(LOG_FILENAME, __LINE__, LogLevel::LOG, fmt, ##__VA_ARGS__)
 
#define B3D_LOG_WARNING(fmt, ...) \
    Logger::log(LOG_FILENAME, __LINE__, LogLevel::WARNING, fmt, ##__VA_ARGS__)
 
#define B3D_LOG_ERROR(fmt, ...)  \
    Logger::log(LOG_FILENAME, __LINE__, LogLevel::ERROR, fmt, ##__VA_ARGS__)
