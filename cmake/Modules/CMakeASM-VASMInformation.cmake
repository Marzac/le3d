set(CMAKE_ASM-VASM_SOURCE_FILE_EXTENSIONS s)

set(CMAKE_ASM-VASM_COMPILE_OBJECT "<CMAKE_ASM-VASM_COMPILER> <INCLUDES> <FLAGS> -m68080 -m68882 -Fhunk -o <OBJECT> <SOURCE>")

# Load the generic ASMInformation file:
set(ASM_DIALECT "-VASM")
include(CMakeASMInformation)
set(ASM_DIALECT)