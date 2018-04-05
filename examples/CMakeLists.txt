if (WIN32)
    add_executable(cube
        cube/cube_win.cpp
    )
else()
    add_executable(cube
        cube/cube_unix.cpp
    )
endif()

set_target_properties(cube PROPERTIES LINK_FLAGS "-m32")
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
