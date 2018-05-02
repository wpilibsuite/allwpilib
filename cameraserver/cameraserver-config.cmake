include(CMakeFindDependencyMacro)
find_dependency(wpiutil)
find_dependency(ntcore)
find_dependency(cscore)
find_dependency(OpenCV)

get_filename_component(SELF_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)
include(${SELF_DIR}/cameraserver.cmake)
