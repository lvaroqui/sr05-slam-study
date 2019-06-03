#!/bin/bash

if [ ! -d "cmake-build-debug" ]; then
  mkdir cmake-build-debug
fi

# Configuring project
cd cmake-build-debug
cmake ..
cd ..

# Building
cmake --build cmake-build-debug --target explorer_controller
