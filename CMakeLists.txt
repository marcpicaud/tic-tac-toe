cmake_minimum_required(VERSION 3.0)

project(tic-tac-toe)

set(SRC_CLIENT client.c)
set(SRC_SRV server.c)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

# CLIENT
add_executable(bin_client ${SRC_CLIENT})
if(WIN32)
  target_link_libraries(bin_client wsock32 ws2_32)
endif()

# SERVER
add_executable(bin_srv ${SRC_SRV})
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(bin_srv ${CMAKE_THREAD_LIBS_INIT})
if(WIN32)

    message(FATAL_ERROR "FUCK YOU ASSHOLE")
  target_link_libraries(bin_client wsock32 ws2_32)
endif()
