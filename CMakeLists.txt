cmake_minimum_required(VERSION 3.10)
project(naive_coro)

set(CMAKE_CXX_STANDARD 11)

FILE(GLOB CORO_LIB "src/*.cpp")

add_library(coro ${CORO_LIB})

add_executable(naive_coro main.cpp)

target_link_libraries(naive_coro coro)