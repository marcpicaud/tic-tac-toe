cmake_minimum_required(VERSION 3.0)

project(tic-tac-toe)

set(SRC_CLIENT client.c)
set(SRC_SRV server.c)

# CLIENT
add_executable(bin_client ${SRC_CLIENT})
if(WIN32)
  target_link_libraries(bin_client wsock32 ws2_32)
endif()

# SERVER
add_executable(bin_srv ${SRC_SRV})
if(WIN32)
  target_link_libraries(bin_client wsock32 ws2_32)
endif()
