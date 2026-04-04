#pragma once

#include <iostream>
#include <string>
#include <chrono>
#include <cstdarg>

// Enum define log levels
enum class LogLevel {
    NONE,
    ERROR,
    WARN,
    INFO,
    DEBUG
};

namespace tools
{
// Create a class for logging objects
class Logger
{
public:

    // Constructors and destructors
    Logger(const std::string &name, LogLevel level = LogLevel::INFO)
        : _name(name), _logLevel(level) {};
    ~Logger() = default;

    // Log methods
    void log(LogLevel level, const char* fmt, ...);
    void setLogLevel(LogLevel level);

private:
    std::string _name; ///< Name of the logger.
    LogLevel _logLevel; ///< Current log level.

    std::string level_to_string(LogLevel level) const;
    const char* get_color_code(LogLevel level);
    std::string vformat(const char* fmt, va_list args);

};

// ========== MACRO LOG ==========
// Logging macros
#define LOG_ERR(logger, fmt, ...) (logger).log(LogLevel::ERROR, fmt, ##__VA_ARGS__)
#define LOG_WRN(logger, fmt, ...) (logger).log(LogLevel::WARN,  fmt, ##__VA_ARGS__)
#define LOG_INF(logger, fmt, ...) (logger).log(LogLevel::INFO,  fmt, ##__VA_ARGS__)
#define LOG_DBG(logger, fmt, ...) (logger).log(LogLevel::DEBUG, fmt, ##__VA_ARGS__)


// /*! \brief rotation value to string
//  * \param rotation the value to convert
//  */
// std::string to_string(ldlidar::ROTATION val);

uint64_t GetSystemTimeStamp(void);
} // namespace tools