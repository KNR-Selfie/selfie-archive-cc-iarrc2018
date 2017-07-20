cmake_minimum_required(VERSION 2.8)
project(sklejanie_obrazu)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

aux_source_directory(. SRC_LIST)
add_executable(${PROJECT_NAME} ${SRC_LIST})

target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})
