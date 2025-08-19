cmake_minimum_required(VERSION 3.5)
project(ldlidar_component)

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
    # ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/include
    ${CMAKE_CURRENT_SOURCE_DIR}/component/include
    # ${CMAKE_CURRENT_SOURCE_DIR}/ldlidar_driver/include/core
    # ${CMAKE_CURRENT_SOURCE_DIR}/ldlidar_driver/include/dataprocess
    # ${CMAKE_CURRENT_SOURCE_DIR}/ldlidar_driver/include/filter
    # ${CMAKE_CURRENT_SOURCE_DIR}/ldlidar_driver/include/logger
    # ${CMAKE_CURRENT_SOURCE_DIR}/ldlidar_driver/include/networkcom
    # ${CMAKE_CURRENT_SOURCE_DIR}/ldlidar_driver/include/serialcom
    ${UDEV_INCLUDE_DIRS}
)

###############################################################################
# Add the component library
add_subdirectory(ldlidar_driver)

###############################################################################
# Creat executable for ldlidar_component
add_executable(${PROJECT_NAME}
    component/src/ldlidar_component.cpp
    component/src/main.cpp
    tools/src/ldlidar_tools.cpp

)

###############################################################################
# Link libraries
target_link_libraries(${PROJECT_NAME} ldlidar )
