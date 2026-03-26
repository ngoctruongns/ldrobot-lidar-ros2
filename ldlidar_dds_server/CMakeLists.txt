cmake_minimum_required(VERSION 3.8)
project(ldlidar_dds_server)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ament_cmake QUIET)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../ldlidar_component/cmake-modules)
find_package(UDEV REQUIRED)
find_package(fastcdr REQUIRED)
find_package(fastrtps REQUIRED)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../ldlidar_component/ldlidar_driver
                 ${CMAKE_CURRENT_BINARY_DIR}/ldlidar_driver)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../ldlidar_component/ldlidar_fastdds
                 ${CMAKE_CURRENT_BINARY_DIR}/ldlidar_fastdds)

add_library(ldlidar_tool STATIC
  ${CMAKE_CURRENT_SOURCE_DIR}/../ldlidar_component/tools/src/ldlidar_tools.cpp
)
target_include_directories(ldlidar_tool PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/../ldlidar_component/tools/include
)
target_link_libraries(ldlidar_tool ${UDEV_LIBRARIES})

add_library(ldlidar_component_core STATIC
  ${CMAKE_CURRENT_SOURCE_DIR}/../ldlidar_component/component/src/ldlidar_component.cpp
)
target_include_directories(ldlidar_component_core PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/../ldlidar_component/component/include
  ${CMAKE_CURRENT_SOURCE_DIR}/../ldlidar_component/tools/include
  ${CMAKE_CURRENT_SOURCE_DIR}/../ldlidar_component/ldlidar_driver/include/core
  ${CMAKE_CURRENT_SOURCE_DIR}/../ldlidar_component/ldlidar_driver/include/dataprocess
  ${CMAKE_CURRENT_SOURCE_DIR}/../ldlidar_component/ldlidar_driver/include/filter
  ${CMAKE_CURRENT_SOURCE_DIR}/../ldlidar_component/ldlidar_driver/include/logger
  ${CMAKE_CURRENT_SOURCE_DIR}/../ldlidar_component/ldlidar_driver/include/networkcom
  ${CMAKE_CURRENT_SOURCE_DIR}/../ldlidar_component/ldlidar_driver/include/serialcom
)
target_link_libraries(ldlidar_component_core
  ldlidar
  ldlidar_tool
  ${UDEV_LIBRARIES}
)

add_executable(ldlidar_dds_server_node
  src/ldlidar_dds_server_publisher.cpp
)

target_link_libraries(ldlidar_dds_server_node
  ldlidar_component_core
  ldlidar_tool
  LidarMsg_lib
)

if(ament_cmake_FOUND)
  install(TARGETS
    ldlidar_dds_server_node
    DESTINATION lib/${PROJECT_NAME}
  )

  ament_package()
else()
  install(TARGETS
    ldlidar_dds_server_node
    DESTINATION bin
  )
endif()
