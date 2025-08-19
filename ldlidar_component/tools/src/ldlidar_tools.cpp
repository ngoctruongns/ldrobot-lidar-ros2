//  Copyright 2024 Walter Lucetti
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
////////////////////////////////////////////////////////////////////////////////

#include "ldlidar_tools.hpp"

namespace tools
{

// Function to log messages based on the current log level
void Logger::log(LogLevel level, const char* fmt, ...)
{
    if (level > _logLevel) {
        return; // Skip logging if the level is lower than the current log level
    }

    // Format the message using variable arguments
    // This allows for formatted strings similar to printf
    // Example: logger.log(LogLevel::INFO, "Value: %d", value);
    va_list args;
    va_start(args, fmt);
    std::string message = vformat(fmt, args);
    va_end(args);

    std::cout << get_color_code(level) << "[" << level_to_string(level) << "] "
              << _name << ": " << message << "\033[0m" << std::endl;
}

std::string Logger::vformat(const char *fmt, va_list args)
{
    char buffer[1024];
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    return std::string(buffer);
}

void Logger::setLogLevel(LogLevel level)
{
    _logLevel = level;
}

std::string Logger::level_to_string(LogLevel level) const
{
    switch (level) {
        case LogLevel::DEBUG: return "DBG";
        case LogLevel::INFO:  return "INF";
        case LogLevel::WARN:  return "WRN";
        case LogLevel::ERROR: return "ERR";
        default:              return "UNK";
    }
}

const char* Logger::get_color_code(LogLevel level)
{
    switch (level) {
        case LogLevel::DEBUG: return "\033[34m"; // Blue
        case LogLevel::INFO:  return "\033[32m"; // Green
        case LogLevel::WARN:  return "\033[33m"; // Yellow
        case LogLevel::ERROR: return "\033[31m"; // Red
        default:              return "\033[0m";  // Reset
    }
}

// Function to get the current system timestamp in nanoseconds
uint64_t GetSystemTimeStamp(void)
{
  std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds> tp =
    std::chrono::time_point_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now());
  auto tmp = std::chrono::duration_cast<std::chrono::nanoseconds>(tp.time_since_epoch());
  return (uint64_t)tmp.count();
}

} // namespace tools