#!/bin/sh

set -e

cd /le3d
mkdir -p build-amiga
cd build-amiga
cmake -DCMAKE_BUILD_TYPE=Release -DLE3D_RENDERER_INTRASTER=On -DAMIGA_TOOLCHAIN_PATH=/opt/amiga/ -DCMAKE_TOOLCHAIN_FILE=../amiga.cmake ..
make