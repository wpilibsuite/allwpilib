set(GCC_COMPILER_VERSION "" CACHE STRING "GCC Compiler version")
set(GNU_MACHINE "arm-frc2021-linux-gnueabi" CACHE STRING "GNU compiler triple")
set(SOFTFP yes)
include("${CMAKE_CURRENT_LIST_DIR}/arm.toolchain.cmake")
