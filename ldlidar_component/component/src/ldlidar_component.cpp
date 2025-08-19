//  Copyright 2024 Walter Lucetti
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
////////////////////////////////////////////////////////////////////////////////

#include "ldlidar_component.hpp"
#include <csignal>
#include <atomic>

using namespace std::placeholders;
using Clock = std::chrono::steady_clock;

namespace ldlidar
{
    static std::atomic<bool> g_stopSignalReceived(false);

    void lidarSignalHandler(int signum)
    {
        if (signum == SIGINT || signum == SIGTERM)
        {
            g_stopSignalReceived = true;
        }
    }

    LdLidarComponent::LdLidarComponent()
    : _logger("LIDAR")
    {
        // Register signal handler for graceful shutdown
        std::signal(SIGINT, lidarSignalHandler);
        std::signal(SIGTERM, lidarSignalHandler);

        // Initialize parameters
        _debugMode = true;
        _lidarModel = "LDLiDAR_LD19";
        _serialPort = "/dev/ldlidar";
        _baudrate = 230400;
        _readTimeOut_msec = 1000;
        _counterclockwise = true;
        _enableAngleCrop = false;
        _angleCropMin = 90.0;
        _angleCropMax = 270.0;
        _bins = 455;
        _rangeMin = 0.03;
        _rangeMax = 12.0;
        _distScale = 0.001;

        // Initialize lidar type
        _lidarType = ldlidar::LDType::LD_19;

        // Initialize diagnostic parameters
        _pubFreq = 0.0;
        _publishing = false;
    }

    LdLidarComponent::~LdLidarComponent()
    {
    }

    bool LdLidarComponent::isProcessRunning()
    {
        return !_threadStop;
    }

    void LdLidarComponent::publishLaserScan(ldlidar::Points2D &src, double lidar_spin_freq)
    {
        float angle_min, angle_max, angle_increment;
        double scan_time;
        Clock::time_point start_scan_time;
        static Clock::time_point end_scan_time;
        static bool first_scan = true;

        int beam_size = 0;

        // With a fixed number of bins the compatibility with SlamToolbox is guaranteed
        if (_bins > 0)
        {
            beam_size = _bins;
        }
        else
        {
            beam_size = static_cast<int>(src.size());
        }

        start_scan_time = Clock::now();
        std::chrono::duration<double> scan_duration = start_scan_time - end_scan_time;
        scan_time = scan_duration.count();

        if (first_scan)
        {
            first_scan = false;
            end_scan_time = start_scan_time;
            return;
        }
        // Adjust the parameters according to the demand
        angle_min = 0;
        angle_max = (2 * M_PI);
        angle_increment = (angle_max - angle_min) / (float)(beam_size - 1);
        // Calculate the number of scanning points
        if (lidar_spin_freq > 0)
        {
            // TODO: Change by message parameter
            LidarMessage msg;
            msg.header_stamp = start_scan_time;
            msg.angle_increment = angle_increment;
            if (beam_size <= 1)
            {
                msg.time_increment = 0;
            }
            else
            {
                msg.time_increment = static_cast<float>(scan_time / (double)(beam_size - 1));
            }
            msg.scan_time = scan_time;
            // First fill all the data with Nan
            msg.ranges.assign(beam_size, std::numeric_limits<float>::quiet_NaN());
            msg.intensities.assign(beam_size, std::numeric_limits<float>::quiet_NaN());
            for (auto point : src)
            {
                float range = point.distance * _distScale; // distance unit transform to meters
                float intensity = point.intensity;         // laser receive intensity
                float dir_angle = point.angle;

                if ((point.distance == 0) && (point.intensity == 0))
                { // filter is handled to  0, Nan will be assigned variable.
                    range = std::numeric_limits<float>::quiet_NaN();
                    intensity = std::numeric_limits<float>::quiet_NaN();
                }

                if (_enableAngleCrop)
                { // Angle crop setting, Mask data within the set angle range
                    if ((dir_angle >= _angleCropMin) && (dir_angle <= _angleCropMax))
                    {
                        range = std::numeric_limits<float>::quiet_NaN();
                        intensity = std::numeric_limits<float>::quiet_NaN();
                    }
                }

                float angle = ANGLE_TO_RADIAN(dir_angle); // Lidar angle unit form degree transform to radian
                int index = static_cast<int>(ceil((angle - angle_min) / angle_increment));
                if (index < beam_size)
                {
                    if (index < 0)
                    {
                        LOG_ERR(_logger, "error index: %d, beam_size: %d, angle: %f, msg.angle_min: %f, msg.angle_increment: %f",
                                index, beam_size, angle, angle_min, angle_increment);
                    }

                    if (_counterclockwise)
                    {
                        int index_anticlockwise = beam_size - index - 1;
                        // If the current content is Nan, it is assigned directly
                        if (std::isnan(msg.ranges[index_anticlockwise]))
                        {
                            msg.ranges[index_anticlockwise] = range;
                        }
                        else
                        { // Otherwise, only when the distance is less than the current
                          //   value, it can be re assigned
                            if (range < msg.ranges[index_anticlockwise])
                            {
                                msg.ranges[index_anticlockwise] = range;
                            }
                        }
                        msg.intensities[index_anticlockwise] = intensity;
                    }
                    else
                    {
                        // If the current content is Nan, it is assigned directly
                        if (std::isnan(msg.ranges[index]))
                        {
                            msg.ranges[index] = range;
                        }
                        else
                        { // Otherwise, only when the distance is less than the current
                          //   value, it can be re assigned
                            if (range < msg.ranges[index])
                            {
                                msg.ranges[index] = range;
                            }
                        }
                        msg.intensities[index] = intensity;
                    }
                }
            }

            // TODO: Publish the message
            // _scanPub->publish(std::move(msg));

            end_scan_time = start_scan_time;
        }
    }

    bool LdLidarComponent::initLidar()
    {
        _lidar = std::make_unique<ldlidar::LDLidarDriver>();

        _lidar->RegisterGetTimestampFunctional(tools::GetSystemTimeStamp);
        _lidar->EnableFilterAlgorithnmProcess(true);

        _logger.setLogLevel(LogLevel::DEBUG); // Set logger to DEBUG level

        if (!initLidarComm())
        {
            return false;
        }

        return true;
    }

    bool LdLidarComponent::initLidarComm()
    {
        if (_lidar->Start(_lidarType, _serialPort, _baudrate, ldlidar::COMM_SERIAL_MODE))
        {
            LOG_INF(_logger, "LDLidar opened on port '%s' *******", _serialPort.c_str());

        }
        else
        {
            LOG_ERR(_logger, "!!! LDLidar not opened !!!");
            exit(EXIT_FAILURE);
        }

        if (_lidar->WaitLidarCommConnect(3000))
        {
            LOG_INF(_logger, " * LDLidar communication OK");
        }
        else
        {
            LOG_ERR(_logger, " !!! LDLidar communication timeout !!!");
            _lidar->Stop();
            exit(EXIT_FAILURE);
        }

        return true;
    }

    void LdLidarComponent::startLidarThread()
    {
        _lidarThread = std::thread(&LdLidarComponent::lidarThreadFunc, this);
    }

    void LdLidarComponent::stopLidarThread()
    {
        if (!_threadStop)
        {
            _threadStop = true;
            LOG_DBG(_logger, "Stopping lidar thread...");
            try
            {
                if (_lidarThread.joinable())
                {
                    _lidarThread.join();
                }
            }
            catch (std::system_error &e)
            {
                LOG_WRN(_logger, "Lidar thread joining exception: %s", e.what());
            }
        }
    }

    void LdLidarComponent::lidarThreadFunc()
    {
        LOG_DBG(_logger, "Lidar thread started");

        _threadStop = false;
        _publishing = false;

        ldlidar::Points2D laser_scan_points;
        double lidar_scan_freq;

        while (1)
        {
            // ----> Interruption check
            if (g_stopSignalReceived)
            {
                LOG_DBG(_logger, "Ctrl+C received: stopping grab thread");
                _threadStop = true;
            }

            if (_threadStop)
            {
                LOG_DBG(_logger, "Lidar thread stopped");
                break;
            }
            // <---- Interruption check
            // Get number of subscribers to the scan topic
            int nSub = 1;    // TODO: Replace with actual subscriber count logic
            if (nSub > 0)
            {
                _publishing = true;
                switch (_lidar->GetLaserScanData(laser_scan_points, _readTimeOut_msec))
                {
                case ldlidar::LidarStatus::NORMAL:
                    _lidar->GetLidarScanFreq(lidar_scan_freq);
                    publishLaserScan(laser_scan_points, lidar_scan_freq);
                    break;
                case ldlidar::LidarStatus::DATA_TIME_OUT:
                    LOG_ERR(_logger, "get ldlidar data is time out, please check your lidar device.");
                    break;
                case ldlidar::LidarStatus::DATA_WAIT:
                    break;
                default:
                    break;
                }
            }
            else
            {
                _publishing = false;
            }

            // Sleep until the next scan is ready
            using std::chrono::nanoseconds;
            if (_lidar->GetLidarScanFreq(lidar_scan_freq) && lidar_scan_freq != 0.0)
            {
                std::this_thread::sleep_for(nanoseconds(int64_t(1e9 / lidar_scan_freq)));
            }
            else
            {
                std::this_thread::sleep_for(nanoseconds(int64_t(1e9 / 10)));
            }
        }

        LOG_DBG(_logger, "Lidar thread finished");
    }

} // namespace ldlidar
