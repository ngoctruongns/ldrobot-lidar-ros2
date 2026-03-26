#include "LidarMsgSubscriber.h"
#include "ldlidar_tools.hpp"

int main()
{
    tools::Logger logger("SUB");

    LidarMsgSubscriber subscriber;
    if (!subscriber.init())
    {
        LOG_ERR(logger, "Failed to initialize LidarMsgSubscriber.");
        return -1;
    }

    subscriber.run();

    return 0;
}
