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

    # Allow unknown warning options
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        target_compile_options(${target} PRIVATE -Wno-unknown-warning-option)
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_compile_options(${target} PRIVATE -Wno-unknown-warning)
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
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND NOT EMSCRIPTEN)
        if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 20.0)
            target_compile_options(
                ${target}
                PRIVATE $<$<COMPILE_LANGUAGE:C>:-Wno-fixed-enum-extension>
            )
        else()
            target_compile_options(${target} PRIVATE $<$<COMPILE_LANGUAGE:C>:-Wno-c23-extensions>)
        endif()
    endif()

    # Suppress warning "implicit conversion may change the meaning of the
    # represented code unit" and "is a C2y extension"
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND NOT EMSCRIPTEN)
        target_compile_options(
            ${target}
            PRIVATE
            -Wno-character-conversion
            -Wno-c2y-extensions
        )
    endif()

    # Compress debug info with GCC
    if(
        (${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR ${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")
        AND ${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU"
    )
        target_compile_options(${target} PRIVATE -gz=zlib)
    endif()
endmacro()
