cmake_minimum_required(VERSION 3.10)
project(OrbbecDaBai)

# 设置C++标准
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#set(CMAKE_CXX_STANDARD 17)

set(OrbbecSDK_DIR "./OrbbecSDK")
# find Orbbec SDK
find_package(OrbbecSDK REQUIRED)
find_package(OpenCV REQUIRED)
find_package(PCL REQUIRED COMPONENTS common io visualization)
find_package(Eigen3 REQUIRED)
find_package(Threads REQUIRED)

IF(NOT WIN32)
    add_definitions(-Wno-format-extra-args)
    SET(SPECIAL_OS_LIBS "pthread" "X11")
ENDIF()

include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${PCL_INCLUDE_DIRS})
include_directories(include)

add_definitions(${PCL_DEFINITIONS})

FILE(GLOB_RECURSE SRC_FILES source/*.cpp)
FILE(GLOB_RECURSE HDR_FILES include/*.hpp)

CUDA_ADD_EXECUTABLE(run main.cpp ${HDR_FILES} ${SRC_FILES})
add_definitions(-std=c++14 -O2)

target_link_libraries(run ${OpenCV_LIBS})
target_link_libraries(run OrbbecSDK::OrbbecSDK)
target_link_libraries(run ${PCL_LIBRARIES})