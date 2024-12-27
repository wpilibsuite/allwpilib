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
        set(WARNING_FLAGS
            /wd4146
            /wd4244
            /wd4251
            /wd4267
            /wd4324
            /D_CRT_SECURE_NO_WARNINGS
            ${WPILIB_TARGET_WARNINGS}
        )
        if(NOT NO_WERROR)
            set(WARNING_FLAGS ${WARNING_FLAGS} /WX)
        endif()

        target_compile_options(${target} PRIVATE ${WARNING_FLAGS})
    endif()

    # Suppress C++-specific OpenCV warning; C compiler rejects it with an error
    # https://github.com/opencv/opencv/issues/20269
    if(UNIX AND NOT APPLE)
        target_compile_options(
            ${target}
            PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-Wno-deprecated-enum-enum-conversion>
        )
    elseif(UNIX AND APPLE)
        target_compile_options(
            ${target}
            PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-Wno-deprecated-anon-enum-enum-conversion>
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
