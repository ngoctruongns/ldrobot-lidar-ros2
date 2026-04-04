
cmake_minimum_required(VERSION 2.8.12)

project("lidar_fastdds")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find requirements
find_package(fastcdr REQUIRED)
find_package(fastrtps REQUIRED)

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/include)

message(STATUS "Create LidarMsg library")

add_library(LidarMsg_lib STATIC
    src/LidarMsg.cxx
    src/LidarMsgPubSubTypes.cxx
    src/LidarMsgPublisher.cxx
    src/LidarMsgSubscriber.cxx
)
target_include_directories(LidarMsg_lib PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_link_libraries(LidarMsg_lib fastcdr fastrtps)

add_executable(LidarMsgDDS src/LidarMsgPubSubMain.cxx)

target_link_libraries(LidarMsgDDS LidarMsg_lib)
