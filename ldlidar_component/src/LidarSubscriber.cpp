#include "LidarMsgSubscriber.h"
#include "ldlidar_tools.hpp"

int main()
{
    tools::Logger logger("SUB");

    LidarMsgSubscriber mysub;
    if (!mysub.init())
    {
        LOG_ERR(logger, "Failed to initialize LidarMsgSubscriber.");
        return -1;
    }

    mysub.run();

    return 0;
}
