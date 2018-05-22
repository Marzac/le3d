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
    set_target_properties(destroyer PROPERTIES LINK_FLAGS "-s -Wl,--gc-sections")
#   target_compile_options(destroyer PUBLIC -m32) 
	target_compile_options(le3d PRIVATE -Wall -Wno-unused-value -Wno-unused-result)
    target_compile_options(destroyer PRIVATE -ffast-math -mfpmath=sse -fno-exceptions -fno-rtti -fno-stack-protector -fno-math-errno -fno-ident -ffunction-sections)
    target_compile_options(destroyer PRIVATE -O2 -s)
    if (WIN32)
        target_compile_options(destroyer PRIVATE -D__MSVCRT_VERSION__=0x0700)
    endif()
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(destroyer PRIVATE -D__MSVCRT_VERSION__=0x0700 -D_CRT_SECURE_NO_WARNINGS=1 -D_USE_MATH_DEFINES=1)
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
