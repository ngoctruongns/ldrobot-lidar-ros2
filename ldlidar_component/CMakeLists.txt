cmake_minimum_required(VERSION 3.8)
project(ldlidar_component)

################################################
## Generate symbols for IDE indexer (VSCode)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(ament_cmake REQUIRED)

# Default to C99
if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 99)
endif()

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake-modules)
find_package(UDEV REQUIRED)
find_package(fastcdr REQUIRED)
find_package(fastrtps REQUIRED)

#############################################
# Include directories
include_directories(
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
target_link_libraries(ldlidar_tool
    ${UDEV_LIBRARIES}
)
target_include_directories(ldlidar_tool PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/tools/include>
    $<INSTALL_INTERFACE:include>
)

add_library(ldlidar_component STATIC
    component/src/ldlidar_component.cpp
)
target_link_libraries(ldlidar_component
    ldlidar
    ldlidar_tool
    ${UDEV_LIBRARIES})
target_include_directories(ldlidar_component PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/component/include>
    $<INSTALL_INTERFACE:include>
)

install(TARGETS
    ldlidar
    ldlidar_tool
    ldlidar_component
    LidarMsg_lib
    EXPORT export_${PROJECT_NAME}
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION lib/${PROJECT_NAME}
)

install(DIRECTORY
    component/include/
    tools/include/
    ldlidar_fastdds/include/
    ldlidar_driver/include/core/
    ldlidar_driver/include/dataprocess/
    ldlidar_driver/include/filter/
    ldlidar_driver/include/logger/
    ldlidar_driver/include/networkcom/
    ldlidar_driver/include/serialcom/
    DESTINATION include
)

ament_export_targets(export_${PROJECT_NAME} HAS_LIBRARY_TARGET)
ament_export_include_directories(include)
ament_export_libraries(ldlidar ldlidar_tool ldlidar_component LidarMsg_lib)
ament_export_dependencies(fastcdr fastrtps)
ament_package()