# Parameters: input output prefix namespace
file(READ ${input} fileHex HEX)
string(LENGTH "${fileHex}" fileHexSize)
math(EXPR fileSize "${fileHexSize} / 2")

get_filename_component(inputBase ${input} NAME)
string(REGEX REPLACE "[^a-zA-Z0-9]" "_" funcName "${inputBase}")
set(funcName "GetResource_${funcName}")

file(
    WRITE
    "${output}"
    "#include <stddef.h>\n#include <string_view>\nextern \"C\" {\nstatic const unsigned char contents[] = {"
)

string(REGEX MATCHALL ".." outputData "${fileHex}")
string(REGEX REPLACE ";" ", 0x" outputData "${outputData}")
file(APPEND "${output}" " 0x${outputData} };\n")
file(
    APPEND
    "${output}"
    "const unsigned char* ${prefix}${funcName}(size_t* len) {\n  *len = ${fileSize};\n  return contents;\n}\n}\n"
)

if(NOT namespace STREQUAL "")
    file(APPEND "${output}" "namespace ${namespace} {\n")
endif()
file(
    APPEND
    "${output}"
    "std::string_view ${funcName}() {\n  return std::string_view(reinterpret_cast<const char*>(contents), ${fileSize});\n}\n"
)
if(NOT namespace STREQUAL "")
    file(APPEND "${output}" "}\n")
endif()
