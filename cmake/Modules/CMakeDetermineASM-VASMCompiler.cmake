set(CMAKE_ASM_VASM_COMPILER_LIST vasm vasmm68k_mot)

if(NOT CMAKE_ASM_VASM_COMPILER)
  find_program(CMAKE_ASM_VASM_COMPILER
    NAMES ${CMAKE_ASM_VASM_COMPILER_LIST}
    PATHS ${_CMAKE_ASM_VASM_COMPILER_PATHS}
    NO_DEFAULT_PATH
    DOC "VASM compiler"
  )
  unset(_CMAKE_ASM_VASM_COMPILER_PATHS)
endif()

# Load the generic DetermineASM compiler file with the DIALECT set properly:
set(ASM_DIALECT "-VASM")
include(CMakeDetermineASMCompiler)
set(ASM_DIALECT)