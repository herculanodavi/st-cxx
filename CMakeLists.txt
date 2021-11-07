cmake_minimum_required(VERSION 3.10)

# set the project name
project(ArmCpp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_CPPCHECK "cppcheck")

list(
        APPEND CMAKE_CXX_CPPCHECK 
            "--enable=warning"
            "--inconclusive"
            "--force" 
            "--inline-suppr")

# add the executable
add_executable(ArmCpp main.cc)

target_include_directories(ArmCpp PUBLIC inc)