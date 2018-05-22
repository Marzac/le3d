############################################################################### 
# le3d - LightEngine 3D  
# Andreas Streichardt <andreas@mop.koeln>
# twitter: @m0ppers
# website: https://mop.koeln
# copyright Andreas Streichardt 2018
# A straightforward C++ 3D software engine for real-time graphics.
# CMakeLists.txt - cube example
############################################################################### 

add_executable(cube cube.cpp)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set_target_properties(cube PROPERTIES LINK_FLAGS "-s -Wl,--gc-sections")
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_include_directories(cube PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/engine/vs)
    set_target_properties(cube PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "$(ProjectDir)")
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
