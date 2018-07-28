cmake_minimum_required(VERSION 3.10)
project(naive_coro)

set(CMAKE_CXX_STANDARD 11)

include_directories(src)

add_library(hook SHARED src/hook.cpp)

add_library(coro SHARED src/manager.cpp src/scheduler.cpp src/timer.cpp src/context.cpp)

add_executable(naive_coro main.cpp)

target_link_libraries(naive_coro hook coro librt.so libpthread.so libdl.so)