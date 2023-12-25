#!/usr/bin/env bash

mkdir build
cd build || exit
cmake ..
cmake --build .
cd ..
mv build/lab_3 .
rm -rf build
