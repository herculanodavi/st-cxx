cmake_minimum_required(VERSION 3.27)

include(cmake/toolchain.cmake)

# set the project name
project(st-cxx)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS True)

# add the executable
add_executable(st-cxx src/main.cpp)

target_include_directories(st-cxx PUBLIC inc)