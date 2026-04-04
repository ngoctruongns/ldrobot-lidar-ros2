cmake_minimum_required(VERSION 3.5)
project(example)

################################################

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

# Default to C99
if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 99)
endif()

# Creat tools library
add_library(tools_lib
  ${CMAKE_CURRENT_SOURCE_DIR}/../tools/src/ldlidar_tools.cpp
)

target_include_directories(tools_lib PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/../tools/include
)

# Add executable target
add_executable(example_node example.cpp)
target_link_libraries(example_node
  tools_lib
)
