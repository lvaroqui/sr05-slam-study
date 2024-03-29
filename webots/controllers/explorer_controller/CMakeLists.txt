cmake_minimum_required(VERSION 3.0)

project(explorer)
find_package (Threads)
find_package(Boost 1.70.0 COMPONENTS system)

# Defining header and source files
set(HEADERS
        ../../../monitor/utils.h
        )

set(SRCS
        explorer_controller.cpp
        src/Rob.cpp
        src/Net.cpp
        src/Exp.cpp
        src/com/UDPClient.cpp
        src/com/UDPServer.cpp
        src/com/AirplugMessage.cpp
        src/com/RobOrd.cpp
        src/com/RobAck.cpp
        src/com/MailBox.cpp
        ../../../monitor/utils.cpp
        )

# Including WeBots
if (DEFINED ENV{WEBOTS_HOME})
    set(WEBOTS_HOME $ENV{WEBOTS_HOME})
else()
    set(WEBOTS_HOME /usr/local/webots)
endif()

set(WEBOTS_INCLUDE
        ${WEBOTS_HOME}/include/controller/cpp/
        )

include_directories(include ${WEBOTS_INCLUDE})

find_library(WEBOTS_CONTROLLER_LIB CppController ${WEBOTS_HOME}/lib)

# Defining executable and linking libraries
add_executable(explorer_controller ${SRCS} ${SRC_CONTROLLER_FILES})
target_link_libraries (explorer_controller
        ${CMAKE_THREAD_LIBS_INIT}
        ${WEBOTS_CONTROLLER_LIB}
        )

# Copy controller to parent folder so that webots finds it
add_custom_command(TARGET explorer_controller POST_BUILD
        COMMAND rm -f ../explorer_controller
        COMMAND cp explorer_controller ../
        )