#include "ldlidar_component.hpp"

int main()
{
    ldlidar::LdLidarComponent lidarComponent;
    tools::Logger logger("MAIN");

    if (!lidarComponent.initLidar())
    {
        LOG_ERR(logger ,"Failed to initialize LIDAR component.");
        return -1;
    }

    lidarComponent.startLidarThread();

    while (lidarComponent.isProcessRunning())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    lidarComponent.stopLidarThread();

    return 0;
}
