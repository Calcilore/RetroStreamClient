#!/bin/bash
mkdir -p build/win
cd build/win

cmake -DCMAKE_TOOLCHAIN_FILE=../../LinuxMingW64.cmake -DCMAKE_BUILD_TYPE=Release ../..
make

