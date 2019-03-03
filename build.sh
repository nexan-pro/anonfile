#!/bin/sh
echo "build"
if [ ! -d build ]; then
   mkdir ./build
   fi
cd ./build
cmake ..
make -j4
