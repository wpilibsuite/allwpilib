project(protoplugin)

set(PROTO_JAR ${CMAKE_CURRENT_SOURCE_DIR}/binary/wpiprotoplugin.jar)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/binary/wpiprotoplugin.sh.in
    ${CMAKE_CURRENT_BINARY_DIR}/wpiprotoplugin.sh
    @ONLY
)
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/binary/wpiprotoplugin.bat.in
    ${CMAKE_CURRENT_BINARY_DIR}/wpiprotoplugin.bat
    @ONLY
)

if(WIN32)
    set(PROTOC_WPILIB_PLUGIN ${CMAKE_CURRENT_BINARY_DIR}/wpiprotoplugin.bat PARENT_SCOPE)
else()
    set(PROTOC_WPILIB_PLUGIN ${CMAKE_CURRENT_BINARY_DIR}/wpiprotoplugin.sh PARENT_SCOPE)
endif()
