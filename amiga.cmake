# AMIGAAAAA
SET(CMAKE_SYSTEM_NAME Amiga)

SET(CMAKE_FIND_ROOT_PATH  ${AMIGA_TOOLCHAIN_PATH})
# specify the cross compiler
SET(CMAKE_C_COMPILER   "${AMIGA_TOOLCHAIN_PATH}/bin/m68k-amigaos-gcc")
SET(CMAKE_ASM-VASM_COMPILER "${AMIGA_TOOLCHAIN_PATH}/bin/vasmm68k_mot")
SET(CMAKE_CXX_COMPILER "${AMIGA_TOOLCHAIN_PATH}/bin/m68k-amigaos-g++")

# newest toolchain works for le3d but the cmake compiler check
# doesn't work due to linker errors :S
set(CMAKE_C_COMPILER_WORKS 1)
set(CMAKE_CXX_COMPILER_WORKS 1)
SET(CMAKE_C_FLAGS "-noixemul -m68040" CACHE STRING "" FORCE)
SET(CMAKE_CXX_FLAGS "-noixemul -m68040" CACHE STRING "" FORCE)

SET(UNIX false)
SET(AMIGA true)