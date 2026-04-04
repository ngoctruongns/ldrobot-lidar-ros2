#include <algorithm>
#include <atomic>
#include <chrono>
#include <csignal>
#include <thread>

#include "LidarMsgPublisher.h"
#include "ldlidar_component.hpp"

namespace
{
std::atomic<bool> g_running{true};

void signalHandler(int signal)
{
    if (signal == SIGINT)
    {
        g_running.store(false);
    }
}
} // namespace

int main()
{
    std::signal(SIGINT, signalHandler);

    ldlidar::LdLidarComponent lidarComponent;
    tools::Logger logger("PUB");

    while (g_running.load() && !lidarComponent.initLidar())
    {
        LOG_ERR(logger, "Failed to initialize LIDAR component, retrying in 3s...");

        // Sleep in small chunks so Ctrl+C can stop promptly.
        for (int i = 0; i < 30 && g_running.load(); ++i)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

    if (!g_running.load())
    {
        LOG_INF(logger, "SIGINT received. Exiting publisher.");
        return 0;
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
