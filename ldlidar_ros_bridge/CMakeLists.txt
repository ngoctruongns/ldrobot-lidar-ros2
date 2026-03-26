cmake_minimum_required(VERSION 3.8)
project(ldlidar_ros_bridge)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(ldlidar_component REQUIRED)

add_executable(ldlidar_ros_bridge_node
  src/ldlidar_ros_bridge.cpp
)

ament_target_dependencies(ldlidar_ros_bridge_node
  rclcpp
  sensor_msgs
  ldlidar_component
)

target_link_libraries(ldlidar_ros_bridge_node
  ldlidar_component::LidarMsg_lib
)

add_executable(ldlidar_ros_bridge_debug_subscriber
  src/ldlidar_dds_debug_subscriber.cpp
)

ament_target_dependencies(ldlidar_ros_bridge_debug_subscriber
  ldlidar_component
)

target_link_libraries(ldlidar_ros_bridge_debug_subscriber
  ldlidar_component::LidarMsg_lib
  ldlidar_component::ldlidar_tool
)

install(TARGETS
  ldlidar_ros_bridge_node
  ldlidar_ros_bridge_debug_subscriber
  DESTINATION lib/${PROJECT_NAME}
)

install(DIRECTORY
  launch
  DESTINATION share/${PROJECT_NAME}
)

ament_package()
