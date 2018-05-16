# Building le3d

le3d is written in C++ and suitable for retrocoding. As such one of the design
goals is to enable compilation on old compilers/toolchains. All you need is a
C++98 capable compiler.

## CMake

The build system being used is cmake.

Running a cmake frontend like cmake-ui (windows) or ccmake on UNIX will show
you a list of noteworthy options and a short explanation.

## General build instructions

To compile the engine follow the instructions for your target system. On a UNIX
system le3d can be built like this:

```bash
cd le3d
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j4
```

This will compile the engine and the examples.

Please note that the example must be executed relative to the assets.

So to run the cube example:

```bash
# we are in the build directory
cd ../examples/cube
../build/examples/cube/cube
```

## Embedding le3d

A simple way to embed le3d in your project is to use cmake in your project.
Then you can clone le3d into your root directory and create a CMakeLists.txt

Here is a template that gets you started:

```
project(my-game CXX)
cmake_minimum_required(VERSION 3.0)
set(CMAKE_CXX_STANDARD 98)

# examples not necessary for your own projects
set(LE3D_BUILD_EXAMPLES Off)
add_subdirectory(le3d)

# your sources
add_executable(main
    main.cpp
)

target_include_directories(main PUBLIC
    ${CMAKE_SOURCE_DIR}
)
target_link_libraries(main
    le3d
)
```

## Platform specific notes

### Amiga

The amiga version is targeting bebbos toolchain:

https://github.com/bebbo/amiga-gcc

Install the toolchain somewhere.

Then you have to run cmake and specify the amiga cross compile toolchain file:

```bash
cd le3d
mkdir build-amiga
cmake -DCMAKE_BUILD_TYPE=Release -DAMIGA_TOOLCHAIN_PATH=<ROOT_TOOLCHAIN_INSTALL_DIR> -DCMAKE_TOOLCHAIN_FILE=../amiga.cmake ../
make -j4
```

