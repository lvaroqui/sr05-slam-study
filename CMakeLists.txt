cmake_minimum_required(VERSION 3.14)
project(etude)

set(CMAKE_CXX_STANDARD 17)

add_executable(etude src/main.cpp src/robot.cpp include/robot.h)