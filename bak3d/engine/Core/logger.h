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

constexpr static int MAX_LOG_ENTRIES = 32;

/*
 * Simple Logger class for logging different levels of logging severity (Log, Warning, Error).
 */
class Logger
{
public:
    static void initialize();
    static void shutdown();
    template<typename... Args>
    static void print_log(const char* file, int line, LogLevel level, const char* fmt, Args... args)
    {
        const char* colour = "\033[0m";
        const char* tag = "LOG";
        switch (level)
        {
            case LogLevel::LOG: colour = "\033[37m"; tag = "LOG";  break;
            case LogLevel::WARNING: colour = "\033[33m"; tag = "WARN"; break;
            case LogLevel::ERROR: colour = "\033[1;31m"; tag = "ERR";  break;
        }

        char header_buffer[512];
        std::string elapsed_time_formatted = get_elapsed_time_formatted();
        int length = std::snprintf(header_buffer, sizeof(header_buffer), "[%s][%s] %s:%d - ", elapsed_time_formatted.c_str(), tag, file, line);

        char log_buffer[1024];
        length = std::snprintf(log_buffer, sizeof(log_buffer), fmt, args...);
        std::string full_log = std::string(header_buffer) + log_buffer + "\n";

        if (log_entries_formatted.size() >= MAX_LOG_ENTRIES)
        {
            log_entries_formatted.erase(log_entries_formatted.begin());
        }

        log_entries_formatted.push_back(full_log);
    }

    static std::vector<std::string>& get_log_entries() { return log_entries_formatted; }
private:
    static std::string get_elapsed_time_formatted();

    static std::chrono::steady_clock::time_point start_time;
    static std::vector<std::string> log_entries_formatted;
};

#define B3D_LOG_INFO(fmt, ...) \
    Logger::print_log(LOG_FILENAME, __LINE__, LogLevel::LOG, fmt, ##__VA_ARGS__)
 
#define B3D_LOG_WARNING(fmt, ...) \
    Logger::print_log(LOG_FILENAME, __LINE__, LogLevel::WARNING, fmt, ##__VA_ARGS__)
 
#define B3D_LOG_ERROR(fmt, ...) \
    Logger::print_log(LOG_FILENAME, __LINE__, LogLevel::ERROR, fmt, ##__VA_ARGS__)
