include(CMakeFindDependencyMacro)
find_dependency(wpiutil)

get_filename_component(SELF_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)
include(${SELF_DIR}/ntcore.cmake)
