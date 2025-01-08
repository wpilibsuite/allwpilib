project(libdatalog)

include(CompileWarnings)

file(
    GLOB libdatalog_src
    src/main/native/cpp/*.cpp
)

add_library(libdatalog ${libdatalog_src})
set_target_properties(wpiutil PROPERTIES DEBUG_POSTFIX "d")

target_compile_features(libdatalog PUBLIC cxx_std_20)
if(MSVC)
    target_compile_options(
        libdatalog
        PUBLIC /permissive- /Zc:preprocessor /Zc:__cplusplus /Zc:throwingNew /MP /bigobj /utf-8
    )
    target_compile_definitions(libdatalog PRIVATE -D_CRT_SECURE_NO_WARNINGS)
endif()
wpilib_target_warnings(libdatalog)

target_include_directories(
    libdatalog
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/include>
)

target_link_libraries(libdatalog PRIVATE wpiutil)
