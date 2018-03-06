project(le3d CXX)
cmake_minimum_required(VERSION 3.4)
set(CMAKE_CXX_STANDARD 11)

option(LE3D_BUILD_EXAMPLES "Build the example programs" ON)
option(LE3D_FAST_MATH "Faster floating point but less precise" ON)

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
        engine/draw_win.cpp
        engine/gamepad_win.cpp
        engine/window_win.cpp
        tools/timing_win.cpp
    )
else()
    find_package(X11 REQUIRED)
    list(APPEND LINK_LIBRARIES
        ${X11_LIBRARIES}
    )
    list(APPEND le3d_INCLUDE_DIRS
        ${X11_INCLUDE_DIR}
    )
    list(APPEND ENGINE_FILES
        engine/draw_unix.cpp
        engine/gamepad_unix.cpp
        engine/window_unix.cpp
        tools/timing_unix.cpp
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

if (LE3D_FAST_MATH)
	if((CMAKE_CXX_COMPILER_ID MATCHES "Clang") OR (CMAKE_CXX_COMPILER_ID MATCHES "GNU"))
		target_compile_options(le3d PUBLIC -ffast-math)
	elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
		target_compile_options(le3d PUBLIC /fp:fast)
	endif()
endif()

if (LE3D_BUILD_EXAMPLES)
    add_subdirectory(examples)   
endif()
