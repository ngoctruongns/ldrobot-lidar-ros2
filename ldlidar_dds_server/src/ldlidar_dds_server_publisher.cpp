#include <algorithm>
#include <chrono>
#include <thread>

#include "LidarMsgPublisher.h"
#include "ldlidar_component.hpp"

int main()
{
    ldlidar::LdLidarComponent lidarComponent;
    tools::Logger logger("PUB");

    if (!lidarComponent.initLidar())
    {
        LOG_ERR(logger ,"Failed to initialize LIDAR component.");
        return -1;
    }

    LidarMsgPublisher mypub;
    if (!mypub.init())
    {
        LOG_ERR(logger, "Failed to initialize LidarMsgPublisher.");
        return -2;
    }

    lidarComponent.startLidarThread();

    mypub.waitSubscribers();
    LOG_INF(logger, "Subscriber connected. Starting data publishing...");
    lidarComponent.setSubscriberCount(1);

    lidarComponent.registerCallback([&mypub](const ldlidar::LidarMessage &msg)
    {
        LidarMessage dds_msg;
        dds_msg.scan_time() = msg.scan_time;

        std::copy_n(msg.ranges.begin(),
                    std::min(msg.ranges.size(), dds_msg.ranges().size()),
                    dds_msg.ranges().begin());

        std::copy_n(msg.intensities.begin(),
                    std::min(msg.intensities.size(), dds_msg.intensities().size()),
                    dds_msg.intensities().begin());

        mypub.pushlishMessageData(dds_msg);
    });

    while (lidarComponent.isProcessRunning())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    lidarComponent.stopLidarThread();

    return 0;
}
