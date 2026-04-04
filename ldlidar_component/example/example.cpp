#include "ldlidar_tools.hpp"

int main() {
    tools::Logger logger("Main", LogLevel::INFO); // Chỉ log INFO, WARN, ERROR

    LOG_DBG(logger, "This debug log will NOT be shown");
    LOG_INF(logger, "System started at PID = %d", 1234);
    LOG_WRN(logger, "Battery level is low: %d%%", 15);
    LOG_ERR(logger, "Cannot open file: %s", "config.txt");

    logger.setLogLevel(LogLevel::DEBUG);

    LOG_DBG(logger, "This debug log WILL be shown after lowering log level");

    return 0;
}