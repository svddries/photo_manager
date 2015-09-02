cmake_minimum_required(VERSION 2.8.3)
project(task_scheduler)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

find_package(OpenCV REQUIRED)
find_package(Boost REQUIRED COMPONENTS filesystem system)

include_directories(
    ${OpenCV_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
)

add_executable(photo_manager src/main.cpp)
target_link_libraries(photo_manager ${OpenCV_LIBRARIES} ${Boost_LIBRARIES} ssl crypto)

