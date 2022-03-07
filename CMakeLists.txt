cmake_minimum_required(VERSION 3.10)

# set the project name
project(ArmCpp)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS True)

# add the executable
add_executable(ArmCpp src/main.cc)

target_include_directories(ArmCpp PUBLIC inc)