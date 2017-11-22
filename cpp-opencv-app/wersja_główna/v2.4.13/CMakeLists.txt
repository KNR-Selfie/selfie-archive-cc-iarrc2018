# Date: 24.10.2017
# Author: Mateusz Grudzien

cmake_minimum_required(VERSION 2.8)
project(24.10.2017)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})
target_link_libraries(${PROJECT_NAME} pthread)
