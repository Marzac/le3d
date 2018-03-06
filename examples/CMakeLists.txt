if (WIN32)
    add_executable(cube
        cube/cube_win.cpp
    )
else()
    add_executable(cube
        cube/cube_unix.cpp
    )
endif()

target_link_libraries(
    cube
    PRIVATE
    le3d
)
target_include_directories(
    cube
    PRIVATE
    ${le3d_INCLUDE_DIRS}
)