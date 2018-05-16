############################################################################### 
# le3d - LightEngine 3D
# Andreas Streichardt <andreas@mop.koeln>
# twitter: @m0ppers
# website: https://mop.koeln
# copyright Andreas Streichardt 2018
# A straightforward C++ 3D software engine for real-time graphics.   
# CMakeLists.txt - root directory 
############################################################################### 

project(le3d C CXX)
cmake_minimum_required(VERSION 3.4)
set(CMAKE_CXX_STANDARD 98)

option(LE3D_BUILD_EXAMPLES "Build the example programs" ON)

SET(le3d_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR})

set(ENGINE_FILES
    engine/bitmap.cpp
    engine/bmpcache.cpp
    engine/bmpfile.cpp
    engine/bset.cpp
    engine/geometry.cpp
    engine/global.cpp
    engine/light.cpp
    engine/mesh.cpp
    engine/meshcache.cpp
    engine/objfile.cpp
    engine/rasterizer_float.cpp
    engine/rasterizer_integer.cpp
    engine/renderer.cpp
    engine/trilist.cpp
    engine/verlist.cpp
    tools/physics.cpp
)

set(LINK_LIBRARIES)

if (WIN32)
    list(APPEND LINK_LIBRARIES
        gdi32
        winmm
    )
    list(APPEND ENGINE_FILES
		engine/system_win.cpp
		engine/draw_win.cpp
        engine/gamepad_win.cpp
        engine/window_win.cpp
        tools/timing_win.cpp
    )
elseif (UNIX)
    find_package(X11 REQUIRED)
    list(APPEND LINK_LIBRARIES
        ${X11_LIBRARIES}
    )
    list(APPEND le3d_INCLUDE_DIRS
        ${X11_INCLUDE_DIR}
    )
    list(APPEND ENGINE_FILES
		engine/system_unix.cpp
        engine/draw_unix.cpp
        engine/gamepad_unix.cpp
        engine/window_unix.cpp
        tools/timing_unix.cpp
    )
elseif (AMIGA)
    list(APPEND ENGINE_FILES
        engine/system_amiga.cpp
        engine/draw_amiga.cpp
        engine/window_amiga.cpp
        tools/timing_amiga.cpp
    )
endif()

add_library(le3d
    ${ENGINE_FILES}
)

target_include_directories(
    le3d
    PUBLIC
    ${le3d_INCLUDE_DIRS}
)

target_link_libraries(
    le3d
    PUBLIC
    ${LINK_LIBRARIES}
)


set(LE3D_CXX_FLAGS_SUGGESTION "")
if (NOT(AMIGA))
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set_target_properties(le3d PROPERTIES LINK_FLAGS "-s -Wl,--gc-sections")
        # we always want these...not user configurable
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-unused-value -Wno-unused-result")
        set(LE3D_CXX_FLAGS_SUGGESTION "${LE3D_CXX_FLAGS_SUGGESTION} -ffast-math -mfpmath=sse -fno-exceptions")
        set(LE3D_CXX_FLAGS_SUGGESTION "${LE3D_CXX_FLAGS_SUGGESTION} -fno-rtti -fno-stack-protector -fno-math-errno")
        set(LE3D_CXX_FLAGS_SUGGESTION "${LE3D_CXX_FLAGS_SUGGESTION} -fno-ident -ffunction-sections")
        if (WIN32)
            set(LE3D_CXX_FLAGS_SUGGESTION "${LE3D_CXX_FLAGS_SUGGESTION} -D__MSVCRT_VERSION__=0x0700")
        endif()
    endif()

    if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        set(LE3D_CXX_FLAGS_SUGGESTION "${LE3D_CXX_FLAGS_SUGGESTION} -D__MSVCRT_VERSION__=0x0700 -D_CRT_SECURE_NO_WARNINGS=1 -D_USE_MATH_DEFINES=1")
        target_include_directories(le3d PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/engine/vs)
    endif()
endif()

set(LE3D_CXX_FLAGS "${LE3D_CXX_FLAGS_SUGGESTION}" CACHE STRING "le3d compile flags")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${LE3D_CXX_FLAGS}")

if (LE3D_BUILD_EXAMPLES)
    add_subdirectory(examples)   
endif()
