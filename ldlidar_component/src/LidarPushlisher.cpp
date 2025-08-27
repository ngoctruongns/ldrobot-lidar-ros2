#include "LidarMsgPublisher.h"
#include "ldlidar_component.hpp"

int main()
{
    // Initialize LIDAR component
    ldlidar::LdLidarComponent lidarComponent;
    tools::Logger logger("PUB");

    if (!lidarComponent.initLidar())
    {
        LOG_ERR(logger ,"Failed to initialize LIDAR component.");
        return -1;
    }

    // Initialize Fast DDS publisher
    LidarMsgPublisher mypub;
    if (!mypub.init())
    {
        LOG_ERR(logger, "Failed to initialize LidarMsgPublisher.");
        return -2;
    }

    // Start LIDAR data acquisition thread
    lidarComponent.startLidarThread();

    // Wait for subscribers to connect
    mypub.waitSubscribers();
    LOG_INF(logger, "Subscriber connected. Starting data publishing...");
    lidarComponent.setSubscriberCount(1); // Set subscriber count to 1

    // Set callback to publish data
    lidarComponent.registerCallback([&mypub](const ldlidar::LidarMessage &msg)
    {
        LidarMessage dds_msg;
        dds_msg.scan_time() = msg.scan_time;

        // copy ranges
        std::copy_n(msg.ranges.begin(),
                    std::min(msg.ranges.size(), dds_msg.ranges().size()),
                    dds_msg.ranges().begin());

        // copy intensities
        std::copy_n(msg.intensities.begin(),
                    std::min(msg.intensities.size(), dds_msg.intensities().size()),
                    dds_msg.intensities().begin());

        mypub.pushlishMessageData(dds_msg);
    });

    // Start publishing loop
    while (lidarComponent.isProcessRunning())
    {
        // mypub.run();
        // lidarComponent.setSubscriberCount(mypub.getMatched());
        // Sleep for a short duration to prevent busy-waiting
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    lidarComponent.stopLidarThread();

    return 0;
}
