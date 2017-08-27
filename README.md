Tic-Tac-Toe
=============

If you want to see debug messages add,

      #define DEBUG

to the start of both client.c and server.c before compiling.

To compile, cd to this directory and run:

      make all

To run the server:

      ./server [some port]

To run the clients:

      ./client [server host] [some port]


# Make it work

## On Windows
Pre-requesite : CMake, MinGW (in your PATH), WIX, and Make

* Remove the build directory
* `mkdir build`
* `cd build`
* `cmake -G "MinGW Makefiles" ..`
* `mingw32-make`
* `mingw32-make install`

To create a .msi installer : `cpack -G WIX --config CPackConfig.cmake`

## On Linux

Pre-requesite : Cmake
* Remove the build directory
* `mkdir build`
* `cd build`
* `cmake ..`
* `make`
* `make install`

To create a linux package : `cpack --config CPackConfig.cmake`
