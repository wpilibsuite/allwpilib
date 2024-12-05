macro(wpilib_target_warnings target)
    if(NOT MSVC)
        set(WARNING_FLAGS
            -Wall
            -pedantic
            -Wextra
            -Wno-unused-parameter
            -Wformat=2
            ${WPILIB_TARGET_WARNINGS}
        )
        if(NOT NO_WERROR)
            set(WARNING_FLAGS ${WARNING_FLAGS} -Werror)
        endif()

        target_compile_options(${target} PRIVATE ${WARNING_FLAGS})
    else()
        target_compile_options(
            ${target}
            PRIVATE
                /wd4146
                /wd4244
                /wd4251
                /wd4267
                /wd4324
                /WX
                /D_CRT_SECURE_NO_WARNINGS
                ${WPILIB_TARGET_WARNINGS}
        )
    endif()

    # Suppress warning "enumeration types with a fixed underlying type are a
    # Clang extension"
    if(APPLE)
        target_compile_options(${target} PRIVATE $<$<COMPILE_LANGUAGE:C>:-Wno-fixed-enum-extension>)
    endif()

    # Compress debug info with GCC
    if(
        (${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR ${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")
        AND ${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU"
    )
        target_compile_options(${target} PRIVATE -gz=zlib)
    endif()
endmacro()
