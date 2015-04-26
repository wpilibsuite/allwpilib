cmake_minimum_required(VERSION 2.8)
project(gz_msgs)

#list all proto files used
get_filename_component(PROTO_DIR src/main/proto ABSOLUTE)
set(msgs
  "${PROTO_DIR}/bool.proto"
  "${PROTO_DIR}/driver-station.proto"
  "${PROTO_DIR}/float64.proto"
  "${PROTO_DIR}/frc_joystick.proto"
)

set (GZ_MSGS_INCLUDE_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated" CACHE FILEPATH "gz_msgs include directory")
set (GZ_MSGS_INCLUDE_SUBDIR "${GZ_MSGS_INCLUDE_DIR}/simulation/gz_msgs")
file(MAKE_DIRECTORY ${GZ_MSGS_INCLUDE_SUBDIR})

set(PROTO_SRCS)
set(PROTO_HDRS)
set(MSGS_HEADER "${GZ_MSGS_INCLUDE_SUBDIR}/msgs.h")
foreach(FIL ${msgs})
  get_filename_component(ABS_FIL ${FIL} ABSOLUTE)
  get_filename_component(FIL_WE ${FIL} NAME_WE)

  list(APPEND PROTO_SRCS "${GZ_MSGS_INCLUDE_SUBDIR}/${FIL_WE}.pb.cc")
  list(APPEND PROTO_HDRS "${GZ_MSGS_INCLUDE_SUBDIR}/${FIL_WE}.pb.h")

  add_custom_command(
    OUTPUT
    "${GZ_MSGS_INCLUDE_SUBDIR}/${FIL_WE}.pb.cc"
    "${GZ_MSGS_INCLUDE_SUBDIR}/${FIL_WE}.pb.h"

    COMMAND  ${PROTOBUF_PROTOC_EXECUTABLE}
    ARGS --cpp_out=${GZ_MSGS_INCLUDE_SUBDIR} --proto_path=${PROTO_DIR} ${ABS_FIL}
    COMMENT "compiling ${ABS_FIL}"
    VERBATIM)
endforeach()

set_source_files_properties(${PROTO_SRCS} ${PROTO_HDRS} PROPERTIES GENERATED TRUE)


###############################################
#Generating msgs.h

#create the message_headers and keep it in cache
set (message_headers "" CACHE INTERNAL "Include dirs description")

#add includes to the msgs.h file
foreach (hdr ${PROTO_HDRS})
  string (REPLACE "${CMAKE_CURRENT_BINARY_DIR}/generated/" "" hdr ${hdr})
  APPEND_TO_CACHED_STRING(message_headers
    "Message Types" "#include \"${hdr}\"\n")
endforeach()

configure_file(msgs.h.in ${MSGS_HEADER})

file(GLOB_RECURSE COM_SRC_FILES msgs/*.cc)
include_directories(msgs ${PROTOBUF_INCLUDE_DIR})
if (WIN32)
  add_library(${PROJECT_NAME} ${PROTO_SRCS} ${SRC_FILES})
else()
  add_library(${PROJECT_NAME} SHARED ${PROTO_SRCS} ${SRC_FILES})
endif()

target_link_libraries(${PROJECT_NAME} ${PROTOBUF_LIBRARIES})
