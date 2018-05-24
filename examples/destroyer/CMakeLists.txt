############################################################################### 
# le3d - LightEngine 3D  
# Andreas Streichardt <andreas@mop.koeln>
# twitter: @m0ppers
# website: https://mop.koeln
# copyright Andreas Streichardt 2018
# A straightforward C++ 3D software engine for real-time graphics.
# CMakeLists.txt - destroyer example
############################################################################### 

add_executable(destroyer destroyer.cpp)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_include_directories(destroyer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/engine/vs)
    set_target_properties(destroyer PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "$(ProjectDir)")
endif()	

target_link_libraries(
    destroyer
    PRIVATE
    le3d
)
target_include_directories(
    destroyer
    PRIVATE
    ${le3d_INCLUDE_DIRS}
)
