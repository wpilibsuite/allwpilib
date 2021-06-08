# Parameters: input output prefix namespace
FILE(READ ${input} fileHex HEX)
STRING(LENGTH "${fileHex}" fileHexSize)
MATH(EXPR fileSize "${fileHexSize} / 2")

GET_FILENAME_COMPONENT(inputBase ${input} NAME)
STRING(REGEX REPLACE "[^a-zA-Z0-9]" "_" funcName "${inputBase}")
SET(funcName "GetResource_${funcName}")

FILE(WRITE "${output}" "#include <stddef.h>\n#include <string_view>\nextern \"C\" {\nstatic const unsigned char contents[] = {")

STRING(REGEX MATCHALL ".." outputData "${fileHex}")
STRING(REGEX REPLACE ";" ", 0x" outputData "${outputData}")
FILE(APPEND "${output}" " 0x${outputData} };\n")
FILE(APPEND "${output}" "const unsigned char* ${prefix}${funcName}(size_t* len) {\n  *len = ${fileSize};\n  return contents;\n}\n}\n")

IF(NOT namespace STREQUAL "")
  FILE(APPEND "${output}" "namespace ${namespace} {\n")
ENDIF()
FILE(APPEND "${output}" "std::string_view ${funcName}() {\n  return std::string_view(reinterpret_cast<const char*>(contents), ${fileSize});\n}\n")
IF(NOT namespace STREQUAL "")
  FILE(APPEND "${output}" "}\n")
ENDIF()
