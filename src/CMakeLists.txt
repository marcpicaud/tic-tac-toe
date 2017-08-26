# CLIENT
add_executable(bin_client client.c)
if(WIN32)
  target_link_libraries(bin_client wsock32 ws2_32)
endif()
install(TARGETS bin_client DESTINATION bin)

# SERVER
add_executable(bin_srv server.c)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(bin_srv ${CMAKE_THREAD_LIBS_INIT})
if(WIN32)
  target_link_libraries(bin_srv wsock32 ws2_32)
endif()
install(TARGETS bin_srv DESTINATION bin)
