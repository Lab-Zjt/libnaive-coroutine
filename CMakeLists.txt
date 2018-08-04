cmake_minimum_required(VERSION 3.10)
project(coronet)

set(CMAKE_CXX_STANDARD 11)

include_directories(src)
file(GLOB SORA_NET_SRC src/net/*.cpp)
file(GLOB CORO_SRC src/coroutine/*.cpp)

add_library(coro_hook SHARED src/hook/hook.cpp)
add_library(coro_net SHARED ${SORA_NET_SRC})
add_library(coro SHARED ${CORO_SRC})

add_executable(new_delete example/new_delete.cpp)
add_executable(echo example/echo.cpp)
add_executable(compute example/compute.cpp)
add_executable(mutex example/mutex.cpp)
target_link_libraries(new_delete coro_hook coro coro_net libdl.so librt.so libpthread.so)
target_link_libraries(echo coro_hook coro coro_net libdl.so librt.so libpthread.so)
target_link_libraries(compute coro_hook coro coro_net libdl.so librt.so libpthread.so)
target_link_libraries(mutex coro_hook coro coro_net libdl.so librt.so libpthread.so)
