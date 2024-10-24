macro(add_doxygen_docs)
    set(dirs
        apriltag
        cameraserver
        cscore
        fieldImages
        hal
        ntcore
        romiVendordep
        wpilibc
        wpilibNewCommands
        wpimath
        wpinet
        wpiutil
        xrpVendordep
    )
    foreach(dir ${dirs})
        list(APPEND docs_dirs ${dir}/src/main/native/include)
        file(GLOB dirs ${dir}/src/main/native/thirdparty/*/include)
        list(FILTER dirs EXCLUDE REGEX eigen|protobuf)
        set(DOXYGEN_EXCLUDE_PATTERNS "*.pb.h" "**/.clang-tidy" "**/.clang-format")

        if(DOCS_WARNINGS_AS_ERRORS)
            set(DOXYGEN_WARN_AS_ERROR "FAIL_ON_WARNINGS_PRINT")
            list(FILTER dirs EXCLUDE REGEX fmt|memory|units)
            list(
                APPEND
                DOXYGEN_EXCLUDE_PATTERNS
                # apriltag
                "apriltag_pose.h"
                # llvm
                "wpi/AlignOf.h"
                "wpi/Casting.h"
                "wpi/Chrono.h"
                "wpi/Compiler.h"
                "wpi/ConvertUTF.h"
                "wpi/DenseMap.h"
                "wpi/DenseMapInfo.h"
                "wpi/Endian.h"
                "wpi/EpochTracker.h"
                "wpi/Errc.h"
                "wpi/Errno.h"
                "wpi/ErrorHandling.h"
                "wpi/bit.h"
                "wpi/fs.h"
                "wpi/FunctionExtras.h"
                "wpi/function_ref.h"
                "wpi/Hashing.h"
                "wpi/iterator.h"
                "wpi/iterator_range.h"
                "wpi/ManagedStatic.h"
                "wpi/MapVector.h"
                "wpi/MathExtras.h"
                "wpi/MemAlloc.h"
                "wpi/PointerIntPair.h"
                "wpi/PointerLikeTypeTraits.h"
                "wpi/PointerUnion.h"
                "wpi/raw_os_ostream.h"
                "wpi/raw_ostream.h"
                "wpi/SmallPtrSet.h"
                "wpi/SmallSet.h"
                "wpi/SmallString.h"
                "wpi/SmallVector.h"
                "wpi/StringExtras.h"
                "wpi/StringMap.h"
                "wpi/SwapByteOrder.h"
                "wpi/type_traits.h"
                "wpi/VersionTuple.h"
                "wpi/WindowsError.h"
                # libuv
                "uv.h"
                "uv/**"
                # json
                "wpi/adl_serializer.h"
                "wpi/byte_container_with_subtype.h"
                "wpi/json.h"
                "wpi/json_fwd.h"
                "wpi/ordered_map.h"
                # mpack
                "wpi/mpack.h"
            )
        endif()
        list(APPEND docs_dirs ${dirs})
        list(APPEND docs_dirs ${dir}/src/generated/main/native/include)
    endforeach()

    set(DOXYGEN_CASE_SENSE_NAMES false)
    set(DOXYGEN_EXTENSION_MAPPING inc=C++ no_extension=C++)
    set(DOXYGEN_EXTRACT_ALL true)
    set(DOXYGEN_EXTRACT_STATIC true)
    set(DOXYGEN_FILE_PATTERNS "*")
    set(DOXYGEN_FULL_PATH_NAMES true)
    set(DOXYGEN_FULL_SIDEBAR false)
    set(DOXYGEN_GENERATE_HTML true)
    set(DOXYGEN_GENERATE_LATEX false)
    set(DOXYGEN_GENERATE_TREEVIEW true)
    set(DOXYGEN_HTML_COLORSTYLE "LIGHT")
    set(DOXYGEN_HTML_EXTRA_STYLESHEET docs/theme.css)
    set(DOXYGEN_JAVADOC_AUTOBRIEF true)
    set(DOXYGEN_ALIASES
        "effects=\\par <i>Effects:</i>^^"
        "notes=\\par <i>Notes:</i>^^"
        "requires=\\par <i>Requires:</i>^^"
        "requiredbe=\\par <i>Required Behavior:</i>^^"
        "concept{2}=<a href=\"md_doc_concepts.html#1\">2</a>"
        "defaultbe=\\par <i>Default Behavior:</i>^^"
    )
    set(DOXYGEN_PROJECT_NAME WPILibC++)
    set(DOXYGEN_PROJECT_NUMBER version)
    set(DOXYGEN_PROJECT_LOGO wpiutil/src/main/native/resources/wpilib-128.png)
    set(DOXYGEN_QUIET true)
    set(DOXYGEN_RECURSIVE true)
    set(DOXYGEN_STRIP_CODE_COMMENTS false)
    set(DOXYGEN_STRIP_FROM_PATH ${docs_dirs})
    set(DOXYGEN_STRIP_FROM_INC_PATH ${docs_dirs})
    set(DOXYGEN_TIMESTAMP "DATETIME")
    set(DOXYGEN_USE_MATHJAX true)
    set(DOXYGEN_WARNINGS false)
    set(DOXYGEN_WARN_IF_INCOMPLETE_DOC true)
    set(DOXYGEN_WARN_IF_UNDOCUMENTED false)
    set(DOXYGEN_WARN_NO_PARAMDOC true)

    set(DOXYGEN_ENABLE_PREPROCESSING true)
    set(DOXYGEN_MACRO_EXPANSION true)
    set(DOXYGEN_EXPAND_ONLY_PREDEF true)
    set(DOXYGEN_PREDEFINED
        "__cplusplus"
        "HAL_ENUM(name)=enum name : int32_t"
        "DOXYGEN"
        "WPI_NOEXCEPT:=noexcept"
        "WPI_SFINAE(x):="
        "WPI_REQUIRES(x):="
        "WPI_REQUIRES_RET(...):="
        "WPI_ENABLE_IF(...):="
        "WPI_CONSTEXPR:=constexpr"
        "WPI_CONSTEXPR_FNC:=constexpr"
        "WPI_IMPL_DEFINED(...):=implementation_defined"
        "WPI_EBO(...):="
    )
    execute_process(COMMAND git describe OUTPUT_VARIABLE version)
    string(SUBSTRING ${version} 1 -1 version)
    set(DOXYGEN_PROJECT_NUMBER ${version})
    doxygen_add_docs(docs ${docs_dirs})
endmacro()
