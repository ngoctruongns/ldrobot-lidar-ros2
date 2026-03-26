#include <limits>
#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/laser_scan.hpp>

#include "LidarMsgSubscriber.h"

class LdLidarRosBridge : public rclcpp::Node
{
public:
    LdLidarRosBridge()
        : Node("ldlidar_ros_bridge")
    {
        publisher_ = create_publisher<sensor_msgs::msg::LaserScan>("ld_lidar_dds", 10);

        if (!subscriber_.init())
        {
            throw std::runtime_error("Failed to initialize LidarMsgSubscriber");
        }

        subscriber_.set_callback([this](const LidarMessage& msg)
        {
            sensor_msgs::msg::LaserScan ros_msg;
            ros_msg.header.stamp = now();
            ros_msg.header.frame_id = "ldlidar_frame";

            constexpr float angle_min = 0.0F;
            constexpr float angle_max = 6.283185307179586F;
            constexpr int bins = 455;

            ros_msg.angle_min = angle_min;
            ros_msg.angle_max = angle_max;
            ros_msg.angle_increment = (angle_max - angle_min) / (bins - 1);
            ros_msg.scan_time = msg.scan_time();
            ros_msg.time_increment = msg.scan_time() / (bins - 1);
            ros_msg.range_min = 0.02F;
            ros_msg.range_max = 12.0F;

            ros_msg.ranges.resize(bins);
            ros_msg.intensities.resize(bins);

            for (size_t i = 0; i < static_cast<size_t>(bins); ++i)
            {
                const auto range = msg.ranges()[i];
                if (range < ros_msg.range_min || range > ros_msg.range_max)
                {
                    ros_msg.ranges[i] = std::numeric_limits<float>::quiet_NaN();
                }
                else
                {
                    ros_msg.ranges[i] = range;
                }

                ros_msg.intensities[i] = msg.intensities()[i];
            }

            publisher_->publish(ros_msg);
        });
    }

private:
    rclcpp::Publisher<sensor_msgs::msg::LaserScan>::SharedPtr publisher_;
    LidarMsgSubscriber subscriber_;
};

int main(int argc, char* argv[])
{
    rclcpp::init(argc, argv);

    try
    {
        auto node = std::make_shared<LdLidarRosBridge>();
        rclcpp::spin(node);
    }
    catch (const std::exception& ex)
    {
        RCLCPP_ERROR(rclcpp::get_logger("ldlidar_ros_bridge"), "%s", ex.what());
        rclcpp::shutdown();
        return 1;
    }

    rclcpp::shutdown();
    return 0;
}
