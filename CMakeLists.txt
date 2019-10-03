cmake_minimum_required(VERSION 3.15)
project(progOOP)

set(CMAKE_CXX_STANDARD 14)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libstdc++ -static-libgcc -static")

add_executable(progOOP main.cpp 2_1.cpp)