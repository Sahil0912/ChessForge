#!/bin/bash

if [ "$1" == "clean" ]; then
    echo "Cleaning build"
    rm -rf build bin
    exit 0
fi
mkdir -p build
cmake -S . -B build #build files in build/
cmake --build build
if [ $? -eq 0 ]; then
    echo "Build Successful! Running the game ..."
    ./build/bin/ChessForge
else
    echo "Build Failed"
fi