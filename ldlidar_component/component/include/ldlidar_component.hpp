/**
 * @file ldlidar_component.hpp
 * @brief Header file for the LdLidarComponent class.
 *
 * This file contains the definition of the LdLidarComponent class, which is a ROS2 lifecycle node
 * for interfacing with LDLidar devices. It includes methods for configuring, activating, deactivating,
 * cleaning up, shutting down, and handling errors in the lifecycle of the node. Additionally, it provides
 * functionality for parameter handling, diagnostic updates, and lidar data processing and publishing.
 *
 * @copyright Copyright 2024 Walter Lucetti
 * @license Licensed under the Apache License, Version 2.0 (the "License");
 *          you may not use this file except in compliance with the License.
 *          You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * @see http://www.apache.org/licenses/LICENSE-2.0
 */


 #pragma once

#include <memory>
#include <thread>
#include "defines.hpp"
#include "ldlidar_driver.h"
#include "ldlidar_tools.hpp"

namespace ldlidar
{
    // Define struct for message parameters
    struct LidarMessage
    {
        std::chrono::time_point<std::chrono::steady_clock> header_stamp; ///< Timestamp
        float angle_increment; ///< Angle increment
        float scan_time; ///< Scan time
        float time_increment; ///< Time increment
        std::vector<float> ranges; ///< Ranges data
        std::vector<float> intensities; ///< Intensities data
    };
    class LdLidarComponent
    {
    public:
        /**
         * @brief Constructor for LdLidarComponent.
         * @param options Node options for configuring the node.
         */
        LdLidarComponent();

        /**
         * @brief Destructor for LdLidarComponent.
         */
        virtual ~LdLidarComponent();

        /**
         * @brief Initialize the lidar device.
         * @return True if successful, false otherwise.
         */
        bool initLidar();

        /**
         * @brief Initialize the lidar communication.
         * @return True if successful, false otherwise.
         */
        bool initLidarComm();

        /**
         * @brief Start the lidar thread.
         */
        void startLidarThread();

        /**
         * @brief Stop the lidar thread.
         */
        void stopLidarThread();

        /**
         * @brief Function for the lidar thread.
         */
        void lidarThreadFunc();

        /**
         * @brief Publish laser scan data.
         * @param src The source points.
         * @param lidar_spin_freq The lidar spin frequency.
         */
        void publishLaserScan(ldlidar::Points2D &src, double lidar_spin_freq);

        bool isProcessRunning();

    private:
        // ----> Parameters
        bool _debugMode = true;                ///< Debug mode flag.
        std::string _lidarModel;               ///< Lidar model.
        std::string _serialPort;               ///< Serial port name.
        int _baudrate = 230400;                ///< Serial baudrate.
        int _readTimeOut_msec = 1000;          ///< Serial read timeout in milliseconds.
        bool _counterclockwise = true;         ///< Rotation direction.
        bool _enableAngleCrop = false;         ///< Enable angle cropping.
        double _angleCropMin = 90.0;           ///< Angle cropping minimum value.
        double _angleCropMax = 270.0;          ///< Angle cropping maximum value.
        int _bins = 455;                       ///< Fixed number of bins.
        double _rangeMin = 0.03;               ///< Minimum range.
        double _rangeMax = 12.0;               ///< Maximum range.
        float _distScale = 0.001;              ///< Scale factor for distance.
        // <---- Parameters

        // ----> Lidar
        std::unique_ptr<ldlidar::LDLidarDriver> _lidar; ///< Lidar driver.
        ldlidar::LDType _lidarType;                     ///< Lidar type.
        // <---- Lidar

        // ----> Threads
        std::thread _lidarThread; ///< Lidar thread.
        bool _threadStop = false; ///< Thread stop flag.
        // <---- Threads

        // ----> Diagnostic
        double _pubFreq;  ///< Publishing frequency.
        bool _publishing; ///< Publishing flag.
        tools::Logger _logger;
    };


    // <---- Template Function definitions

} // namespace ldlidar
