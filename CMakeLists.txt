cmake_minimum_required(VERSION 3.0)
project(tic-tac-toe)
# The version number.
set (tic-tac-toe_VERSION_MAJOR 1)
set (tic-tac-toe_VERSION_MINOR 0)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
add_subdirectory(src)

# build a CPack driven installer package
include (InstallRequiredSystemLibraries)
set (CPACK_RESOURCE_FILE_LICENSE  
     "${CMAKE_CURRENT_SOURCE_DIR}/License.txt")
set (CPACK_PACKAGE_VERSION_MAJOR "${tic-tac-toe_VERSION_MAJOR}")
set (CPACK_PACKAGE_VERSION_MINOR "${tic-tac-toe_VERSION_MINOR}")
include (CPack)
