project(le3d CXX)
cmake_minimum_required(VERSION 3.4)
set(CMAKE_CXX_STANDARD 11)

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
)

if (WIN32)
    list(APPEND ENGINE_FILES
        engine/draw_win.cpp
        engine/gamepad_win.cpp
        engine/window_win.cpp
    )
else()
    list(APPEND ENGINE_FILES
        engine/draw_unix.cpp
        engine/gamepad_unix.cpp
        engine/window_unix.cpp
    )
endif()

add_library(le3d
    ${ENGINE_FILES}
)