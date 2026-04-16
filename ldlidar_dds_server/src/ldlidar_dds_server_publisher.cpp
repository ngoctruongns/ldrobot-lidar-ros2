#include <algorithm>
#include <atomic>
#include <chrono>
#include <csignal>
#include <memory>
#include <thread>

#include "LidarMsgPublisher.h"
#include "ldlidar_component.hpp"

namespace
{
std::atomic<bool> g_running{true};

void signalHandler(int signal)
{
    if (signal == SIGINT || signal == SIGTERM)
    {
        g_running.store(false);
    }
}
} // namespace

int main()
{
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);

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

    lidarComponent.startLidarThread();
    lidarComponent.setSubscriberCount(1);

    // DDS discovery loop: recreate the DDS participant internally whenever
    // multicast discovery fails (common on boot before routing table is ready).
    // The lidar thread keeps running uninterrupted across retries.
    const int SUBSCRIBER_TIMEOUT_SEC = 10;

    while (g_running.load() && lidarComponent.isProcessRunning())
    {
        // Create a new DDS publisher (shared_ptr so the callback can safely
        // outlive one iteration while the next participant is being created).
        auto mypub = std::make_shared<LidarMsgPublisher>();
        if (!mypub->init())
        {
            LOG_ERR(logger, "Failed to initialize LidarMsgPublisher, retrying in 3s...");
            for (int i = 0; i < 30 && g_running.load(); ++i)
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }

        // Capture mypub by value so the shared_ptr stays alive in the callback
        // even after the local variable goes out of scope during recreation.
        lidarComponent.registerCallback([mypub](const ldlidar::LidarMessage &msg)
        {
            LidarMessage dds_msg;
            dds_msg.scan_time() = msg.scan_time;

            std::copy_n(msg.ranges.begin(),
                        std::min(msg.ranges.size(), dds_msg.ranges().size()),
                        dds_msg.ranges().begin());

            std::copy_n(msg.intensities.begin(),
                        std::min(msg.intensities.size(), dds_msg.intensities().size()),
                        dds_msg.intensities().begin());

            mypub->pushlishMessageData(dds_msg);
        });

        LOG_INF(logger, "LiDAR publishing started. Waiting for DDS subscriber to connect...");

        // Wait for a subscriber with timeout; on timeout recreate the participant.
        auto start_time = std::chrono::steady_clock::now();
        bool connected = false;

        while (g_running.load() && lidarComponent.isProcessRunning())
        {
            if (mypub->getMatched() > 0)
            {
                LOG_INF(logger, "DDS subscriber connected.");
                connected = true;
                break;
            }

            auto elapsed = std::chrono::steady_clock::now() - start_time;
            if (std::chrono::duration_cast<std::chrono::seconds>(elapsed).count() >= SUBSCRIBER_TIMEOUT_SEC)
            {
                LOG_WRN(logger, "No DDS subscriber after %ds — recreating DDS participant...", SUBSCRIBER_TIMEOUT_SEC);
                break; // mypub goes out of scope → DDS participant destroyed → recreate
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        if (!connected)
            continue; // recreate mypub and try again

        // Subscriber is connected — keep running until stopped.
        while (g_running.load() && lidarComponent.isProcessRunning())
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    lidarComponent.stopLidarThread();

    return 0;
}
