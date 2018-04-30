if (WIN32)
    add_executable(cube
        cube/cube_win.cpp
    )
else()
    add_executable(cube
        cube/cube_unix.cpp
    )
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	set_target_properties(cube PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "$(ProjectDir)cube")
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
