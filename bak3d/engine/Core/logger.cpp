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

#include "logger.h"

using namespace std;

chrono::steady_clock::time_point Logger::start_time = chrono::steady_clock::now();
vector<LogEntry> Logger::log_entries = {};

void Logger::initialize()
{
    start_time = chrono::steady_clock::now();
    log_entries.clear();
}

void Logger::shutdown()
{
    log_entries.clear();
    B3D_LOG_INFO("Shutting down engine. Time passed: %s", get_elapsed_time_formatted().c_str());
}

string Logger::get_elapsed_time_formatted()
{
    const auto now = chrono::steady_clock::now();
    const auto difference = now - start_time;

    const auto total_ms = chrono::duration_cast<chrono::milliseconds>(difference).count();

    const long ms = total_ms % 1000;
    const long total_secs = total_ms / 1000;
    const long s = total_secs % 60;
    const long total_mins = total_secs / 60;
    const long m = total_mins % 60;
    const long h = total_mins / 60;

    char time_buffer[32];
    // Format: HH:MM:SS:mmm (0-padded to 2 digits for time, 3 for ms)
    snprintf(time_buffer, sizeof(time_buffer), "%02ld:%02ld:%02ld:%03ld", h, m, s, ms);

    return time_buffer;
}
