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
add_executable(http example/http.cpp)

find_package(OpenSSL REQUIRED)
set(OPENSSL_USE_SHARED_LIB TRUE)


target_link_libraries(coro_hook libdl.so)
target_link_libraries(coro coro_hook coro_net librt.so libpthread.so OpenSSL::SSL OpenSSL::Crypto)

target_link_libraries(new_delete coro_hook coro)
target_link_libraries(echo coro_hook coro)
target_link_libraries(compute coro_hook coro)
target_link_libraries(mutex coro_hook coro)
target_link_libraries(http coro_hook coro)

