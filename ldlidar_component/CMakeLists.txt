cmake_minimum_required(VERSION 3.5)
project(ldlidar_sensor)

################################################
## Generate symbols for IDE indexer (VSCode)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Default to C99
if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 99)
endif()

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake-modules)

#############################################
# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/include
    ${CMAKE_CURRENT_SOURCE_DIR}/component/include
    ${CMAKE_CURRENT_SOURCE_DIR}/ldlidar_driver/include/core
    ${CMAKE_CURRENT_SOURCE_DIR}/ldlidar_driver/include/dataprocess
    ${CMAKE_CURRENT_SOURCE_DIR}/ldlidar_driver/include/filter
    ${CMAKE_CURRENT_SOURCE_DIR}/ldlidar_driver/include/logger
    ${CMAKE_CURRENT_SOURCE_DIR}/ldlidar_driver/include/networkcom
    ${CMAKE_CURRENT_SOURCE_DIR}/ldlidar_driver/include/serialcom
    ${CMAKE_CURRENT_SOURCE_DIR}/ldlidar_fastdds/include
    ${UDEV_INCLUDE_DIRS}
)

###############################################################################
# Add the component library
add_subdirectory(ldlidar_driver)
add_subdirectory(ldlidar_fastdds)

# Creat the shared library for component
add_library(ldlidar_tool STATIC
    tools/src/ldlidar_tools.cpp
)

add_library(ldlidar_component STATIC
    component/src/ldlidar_component.cpp
)
target_link_libraries(ldlidar_component
    ldlidar
    LidarMsg_lib
    ldlidar_tool
    ${UDEV_LIBRARIES})

###############################################################################
# Creat executable for
add_executable(${PROJECT_NAME}_publisher
    src/LidarPushlisher.cpp
)

add_executable(${PROJECT_NAME}_subscriber
    src/LidarSubscriber.cpp
)

###############################################################################
# Link libraries
target_link_libraries(${PROJECT_NAME}_publisher
    ldlidar_component
    ldlidar
    LidarMsg_lib
    ldlidar_tool
    ${UDEV_LIBRARIES}
)

target_link_libraries(${PROJECT_NAME}_subscriber
    LidarMsg_lib
    ldlidar_tool
)