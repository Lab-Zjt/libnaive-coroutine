cmake_minimum_required(VERSION 3.10)
project(coronet)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-Wall -Wno-sign-compare -Wno-pointer-arith")

include_directories(src)
include_directories(3rdparty)
file(GLOB SORA_NET_SRC src/net/*.cpp)
file(GLOB CORO_SRC src/coroutine/*.cpp)

#add_library(coro_hook SHARED src/hook/hook.cpp)
#add_library(coro_net SHARED ${SORA_NET_SRC})
#add_library(coro SHARED ${CORO_SRC})

add_library(coro SHARED src/hook/hook.cpp ${CORO_SRC} ${SORA_NET_SRC})

add_executable(new_delete example/new_delete.cpp)
add_executable(echo example/echo.cpp)
add_executable(compute example/compute.cpp)
add_executable(mutex example/mutex.cpp)
add_executable(http example/http.cpp)
add_executable(channel example/channel.cpp)
add_executable(sleep example/sleep.cpp)
add_executable(spider example/spider.cpp)
add_executable(condvar example/condvar.cpp)
add_executable(barrier example/barrier.cpp)

find_package(OpenSSL REQUIRED)
set(OPENSSL_USE_SHARED_LIB TRUE)

target_link_libraries(coro libdl.so librt.so libpthread.so OpenSSL::SSL OpenSSL::Crypto)

target_link_libraries(new_delete coro)
target_link_libraries(echo coro)
target_link_libraries(compute coro)
target_link_libraries(mutex coro)
target_link_libraries(http coro)
target_link_libraries(channel coro)
target_link_libraries(sleep coro)
target_link_libraries(spider coro)
target_link_libraries(condvar coro)
target_link_libraries(barrier coro)
