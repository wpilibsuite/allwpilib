project(wpiutil)

include(SubDirList)
include(GenResources)
include(CompileWarnings)
include(AddTest)
include(DownloadAndCheck)

file(
    GLOB wpiutil_jni_src
    src/main/native/cpp/jni/WPIUtilJNI.cpp
    src/main/native/cpp/jni/DataLogJNI.cpp
)

# Java bindings
if(WITH_JAVA)
    include(UseJava)

    if(NOT EXISTS "${WPILIB_BINARY_DIR}/wpiutil/thirdparty/jackson/jackson-core-2.15.2.jar")
        set(BASE_URL "https://search.maven.org/remotecontent?filepath=")
        set(JAR_ROOT "${WPILIB_BINARY_DIR}/wpiutil/thirdparty/jackson")

        message(STATUS "Downloading Jackson jarfiles...")

        download_and_check(
            "${BASE_URL}com/fasterxml/jackson/core/jackson-core/2.15.2/jackson-core-2.15.2.jar"
            "${JAR_ROOT}/jackson-core-2.15.2.jar"
        )
        download_and_check(
            "${BASE_URL}com/fasterxml/jackson/core/jackson-databind/2.15.2/jackson-databind-2.15.2.jar"
            "${JAR_ROOT}/jackson-databind-2.15.2.jar"
        )
        download_and_check(
            "${BASE_URL}com/fasterxml/jackson/core/jackson-annotations/2.15.2/jackson-annotations-2.15.2.jar"
            "${JAR_ROOT}/jackson-annotations-2.15.2.jar"
        )

        message(STATUS "All files downloaded.")
    endif()

    file(GLOB JACKSON_JARS ${WPILIB_BINARY_DIR}/wpiutil/thirdparty/jackson/*.jar)

    if(NOT EXISTS "${WPILIB_BINARY_DIR}/wpiutil/thirdparty/quickbuf/quickbuf-runtime-1.3.3.jar")
        set(BASE_URL "https://search.maven.org/remotecontent?filepath=")
        set(JAR_ROOT "${WPILIB_BINARY_DIR}/wpiutil/thirdparty/quickbuf")

        message(STATUS "Downloading Quickbuf jarfile...")
        download_and_check(
            "${BASE_URL}us/hebi/quickbuf/quickbuf-runtime/1.3.3/quickbuf-runtime-1.3.3.jar"
            "${JAR_ROOT}/quickbuf-runtime-1.3.3.jar"
        )

        message(STATUS "Downloaded.")
    endif()

    file(GLOB QUICKBUF_JAR ${WPILIB_BINARY_DIR}/wpiutil/thirdparty/quickbuf/*.jar)

    set(CMAKE_JNI_TARGET true)

    file(GLOB_RECURSE JAVA_SOURCES src/main/java/*.java)

    add_jar(
        wpiutil_jar
        ${JAVA_SOURCES}
        INCLUDE_JARS ${JACKSON_JARS} ${QUICKBUF_JAR}
        OUTPUT_NAME wpiutil
        OUTPUT_DIR ${WPILIB_BINARY_DIR}/${java_lib_dest}
        GENERATE_NATIVE_HEADERS wpiutil_jni_headers
    )
    set_property(TARGET wpiutil_jar PROPERTY FOLDER "java")

    install_jar(wpiutil_jar DESTINATION ${java_lib_dest})
    install_jar_exports(TARGETS wpiutil_jar FILE wpiutil_jar.cmake DESTINATION share/wpiutil)

    add_library(wpiutiljni ${wpiutil_jni_src})
    wpilib_target_warnings(wpiutiljni)
    target_link_libraries(wpiutiljni PUBLIC wpiutil)

    set_property(TARGET wpiutiljni PROPERTY FOLDER "libraries")

    target_link_libraries(wpiutiljni PRIVATE wpiutil_jni_headers)
    add_dependencies(wpiutiljni wpiutil_jar)

    install(TARGETS wpiutiljni EXPORT wpiutiljni)
    export(TARGETS wpiutiljni FILE wpiutiljni.cmake NAMESPACE wpiutiljni::)
endif()

if(WITH_JAVA_SOURCE)
    include(UseJava)
    include(CreateSourceJar)
    add_source_jar(
        wpiutil_src_jar
        BASE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/src/main/java
        OUTPUT_NAME wpiutil-sources
        OUTPUT_DIR ${WPILIB_BINARY_DIR}/${java_lib_dest}
    )
    set_property(TARGET wpiutil_src_jar PROPERTY FOLDER "java")

    install_jar(wpiutil_src_jar DESTINATION ${java_lib_dest})
endif()

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

if(NOT MSVC AND NOT APPLE AND NOT ANDROID)
    find_library(ATOMIC NAMES atomic libatomic.so.1)
    if(ATOMIC)
        message(STATUS "Found libatomic: ${ATOMIC}")
    else()
        message(STATUS "libatomic not found. If build fails, install libatomic")
    endif()
endif()

generate_resources(src/main/native/resources generated/main/cpp WPI wpi wpiutil_resources_src)

file(
    GLOB_RECURSE wpiutil_native_src
    src/main/native/cpp/*.cpp
    src/main/native/thirdparty/json/cpp/*.cpp
    src/main/native/thirdparty/llvm/cpp/*.cpp
    src/main/native/thirdparty/mpack/src/*.cpp
    src/main/native/thirdparty/nanopb/src/*.cpp
)
list(REMOVE_ITEM wpiutil_native_src ${wpiutil_jni_src})
if(NOT WITH_PROTOBUF)
    list(FILTER wpiutil_native_src EXCLUDE REGEX "/protobuf/")
    # Don't filter out protobuf cpp file, it only uses nanopb
    list(APPEND wpiutil_native_src src/main/native/cpp/protobuf/Protobuf.cpp)
endif()
file(GLOB_RECURSE wpiutil_unix_src src/main/native/unix/*.cpp)
file(GLOB_RECURSE wpiutil_linux_src src/main/native/linux/*.cpp)
file(GLOB_RECURSE wpiutil_macos_src src/main/native/macOS/*.cpp)
file(GLOB_RECURSE wpiutil_windows_src src/main/native/windows/*.cpp)

file(GLOB fmtlib_native_src src/main/native/thirdparty/fmtlib/src/*.cpp)
file(GLOB_RECURSE memory_native_src src/main/native/thirdparty/memory/src/*.cpp)

add_library(wpiutil ${wpiutil_native_src} ${memory_native_src} ${wpiutil_resources_src})
set_target_properties(wpiutil PROPERTIES DEBUG_POSTFIX "d")

set_property(TARGET wpiutil PROPERTY FOLDER "libraries")

target_compile_features(wpiutil PUBLIC cxx_std_20)
if(MSVC)
    target_compile_options(
        wpiutil
        PUBLIC /permissive- /Zc:preprocessor /Zc:__cplusplus /Zc:throwingNew /MP /bigobj /utf-8
    )
    target_compile_definitions(wpiutil PRIVATE -D_CRT_SECURE_NO_WARNINGS)
endif()
wpilib_target_warnings(wpiutil)
if(WITH_PROTOBUF)
    target_link_libraries(wpiutil protobuf::libprotobuf Threads::Threads ${CMAKE_DL_LIBS})
else()
    target_link_libraries(wpiutil Threads::Threads ${CMAKE_DL_LIBS})
    target_compile_definitions(wpiutil PUBLIC NO_PROTOBUF)
endif()

if(ATOMIC)
    target_link_libraries(wpiutil ${ATOMIC})
endif()

if(NOT USE_SYSTEM_FMTLIB)
    target_sources(wpiutil PRIVATE ${fmtlib_native_src})
    install(
        DIRECTORY src/main/native/thirdparty/fmtlib/include/
        DESTINATION "${include_dest}/wpiutil"
    )
    target_include_directories(
        wpiutil
        PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/thirdparty/fmtlib/include>
    )
else()
    find_package(fmt CONFIG REQUIRED)
    target_link_libraries(wpiutil fmt::fmt)
    if(MSVC)
        get_target_property(fmt_includes fmt::fmt INTERFACE_INCLUDE_DIRECTORIES)
        foreach(dir ${fmt_includes})
            target_compile_options(wpiutil PUBLIC /external:I "${dir}")
        endforeach()
        target_compile_options(wpiutil PUBLIC /external:W0)
    endif()
endif()

if(MSVC)
    target_sources(wpiutil PRIVATE ${wpiutil_windows_src})
else()
    target_sources(wpiutil PRIVATE ${wpiutil_unix_src})
    if(APPLE)
        target_sources(wpiutil PRIVATE ${wpiutil_macos_src})
    else()
        target_sources(wpiutil PRIVATE ${wpiutil_linux_src})
    endif()
endif()

install(
    DIRECTORY
        src/main/native/include/
        src/main/native/thirdparty/argparse/include/
        src/main/native/thirdparty/expected/include/
        src/main/native/thirdparty/json/include/
        src/main/native/thirdparty/llvm/include/
        src/main/native/thirdparty/memory/include/
        src/main/native/thirdparty/mpack/include/
        src/main/native/thirdparty/nanopb/include/
        src/main/native/thirdparty/sigslot/include/
    DESTINATION "${include_dest}/wpiutil"
)
target_include_directories(
    wpiutil
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/thirdparty/argparse/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/thirdparty/expected/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/thirdparty/json/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/thirdparty/llvm/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/thirdparty/memory/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/thirdparty/mpack/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/thirdparty/nanopb/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/thirdparty/sigslot/include>
        $<INSTALL_INTERFACE:${include_dest}/wpiutil>
)

install(TARGETS wpiutil EXPORT wpiutil)
export(TARGETS wpiutil FILE wpiutil.cmake NAMESPACE wpiutil::)

configure_file(wpiutil-config.cmake.in ${WPILIB_BINARY_DIR}/wpiutil-config.cmake)
install(FILES ${WPILIB_BINARY_DIR}/wpiutil-config.cmake DESTINATION share/wpiutil)
install(EXPORT wpiutil DESTINATION share/wpiutil)

subdir_list(wpiutil_examples "${CMAKE_CURRENT_SOURCE_DIR}/examples")
foreach(example ${wpiutil_examples})
    file(GLOB wpiutil_example_src examples/${example}/*.cpp)
    if(wpiutil_example_src)
        add_executable(wpiutil_${example} ${wpiutil_example_src})
        wpilib_target_warnings(wpiutil_${example})
        target_link_libraries(wpiutil_${example} wpiutil)
        set_property(TARGET wpiutil_${example} PROPERTY FOLDER "examples")
    endif()
endforeach()

if(WITH_TESTS)
    file(GLOB_RECURSE wpiutil_testlib_src src/test/native/include/*.h)
    add_library(wpiutil_testlib INTERFACE ${wpiutil_test_src})
    target_include_directories(wpiutil_testlib INTERFACE src/test/native/include)

    wpilib_add_test(wpiutil src/test/native/cpp)
    target_include_directories(wpiutil_test PRIVATE src/generated/test/native/cpp)
    file(GLOB_RECURSE wpiutil_nanopb_test_src src/generated/test/native/cpp/*.cpp)
    target_sources(wpiutil_test PRIVATE ${wpiutil_nanopb_test_src})
    target_link_libraries(wpiutil_test wpiutil googletest wpiutil_testlib)
    if(MSVC)
        target_compile_options(wpiutil_test PRIVATE /utf-8)
    endif()
endif()

# Copied from https://github.com/foonathan/memory/blob/016c9fbd61b57ed2058551dcf225c15a0e716cce/cmake/get_container_node_sizes.cmake#L7
# We need to capture this outside of a function as
# CMAKE_CURRENT_LIST_DIR reflects the current CMakeLists.txt file when
# a function is executed, but reflects this directory while this file
# is being processed.
set(_THIS_MODULE_DIR ${CMAKE_CURRENT_LIST_DIR})

if(NOT DEFINED _DEBUG_GET_CONTAINER_NODE_SIZES)
    set(_DEBUG_GET_CONTAINER_NODE_SIZES OFF)
endif()

function(_gcns_debug_message)
    if(_DEBUG_GET_CONTAINER_NODE_SIZES)
        message("${ARGV}")
    endif()
endfunction()

# This function will return the alignment of the C++ type specified in
# 'type', the result will be in 'result_var'.
function(get_alignof_type type result_var)
    # We expect this compilation to fail - the purpose of this is to
    # generate a compile error on a generated tyoe
    # "align_of<type,value>" that is the alignment of the specified
    # type.
    #
    # See the contents of get_align_of.cpp for more details.
    try_compile(
        align_result
        ${CMAKE_CURRENT_BINARY_DIR}
        ${_THIS_MODULE_DIR}/src/main/native/thirdparty/memory/src/get_align_of.cpp
        COMPILE_DEFINITIONS "-DTEST_TYPE=${type}"
        OUTPUT_VARIABLE align_output
        CXX_STANDARD 11
        CXX_STANDARD_REQUIRED TRUE
    )

    # Look for the align_of<..., ##> in the compiler error output
    string(REGEX MATCH "align_of<.*,[ ]*([0-9]+)[ul ]*>" align_of_matched ${align_output})

    if(align_of_matched)
        set(${result_var} ${CMAKE_MATCH_1} PARENT_SCOPE)
    else()
        message(
            FATAL_ERROR
            "Unable to determine alignment of C++ type ${type} - no error text matching align_of<..., ##> in compiler output |${align_output}|"
        )
    endif()
endfunction()

# This function will return a list of C++ types with unique alignment
# values, covering all possible alignments supported by the currently
# configured C++ compiler.
#
# The variable named in 'result_types' will contain a list of types,
# and 'result_alignments' will contain a parallel list of the same
# size that is the aligment of each of the matching types.
function(unique_aligned_types result_types result_alignments)
    # These two lists will contain a set of types with unique alignments.
    set(alignments)
    set(types)

    set(all_types
        char
        bool
        short
        int
        long
        LONG_LONG
        float
        double
        LONG_DOUBLE
    )
    foreach(type IN LISTS all_types)
        get_alignof_type("${type}" alignment)
        _gcns_debug_message("Alignment of '${type}' is '${alignment}'")

        if(NOT ${alignment} IN_LIST alignments)
            list(APPEND alignments ${alignment})
            list(APPEND types ${type})
        endif()
    endforeach()

    set(${result_types} ${types} PARENT_SCOPE)
    set(${result_alignments} ${alignments} PARENT_SCOPE)
endfunction()

# This function will return node sizes for the requested container
# when created with the specified set of types.
#
# 'container' must be one of the container types supported by
# get_node_size.cpp (see that file for details)
#
# 'types' is a list of C++ types to hold in the container to measure
# the node size
#
# 'align_result_var' will contain the list of alignments of contained
# types used.
#
# 'nodesize_result_var' will contain the list of node sizes, one entry
# for each alignment/type
function(get_node_sizes_of container types align_result_var nodesize_result_var)
    set(alignments)
    set(node_sizes)

    foreach(type IN LISTS types)
        # We expect this to fail - the purpose of this is to generate
        # a compile error on a generated type
        # "node_size_of<type_size,node_size,is_node_size>" that is the
        # alignment of the specified type.
        try_compile(
            nodesize_result
            ${CMAKE_CURRENT_BINARY_DIR}
            ${_THIS_MODULE_DIR}/src/main/native/thirdparty/memory/src/get_node_size.cpp
            COMPILE_DEFINITIONS "-D${container}=1" "-DTEST_TYPE=${type}"
            OUTPUT_VARIABLE nodesize_output
            CXX_STANDARD 11
            CXX_STANDARD_REQUIRED TRUE
        )

        if(NOT nodesize_output)
            message(
                FATAL_ERROR
                "Unable to determine node size of C++ container ${container} holding type ${type} - no error text matching node_size_of<##, ##, true> in compiler output |${nodesize_output}|"
            )
        endif()

        # Find the instance of node_size_of<##, ##, true> in the
        # compiler error output - the first number is the alignment,
        # and the second is the node size.
        string(
            REGEX MATCH
            "node_size_of<[ ]*([0-9]+)[ul ]*,[ ]*([0-9]+)[ul ]*,[ ]*true[ ]*>"
            node_size_of_match
            ${nodesize_output}
        )

        if(node_size_of_match)
            # Extract the alignment and node size
            if(NOT ${CMAKE_MATCH_1} IN_LIST alignments)
                list(APPEND alignments ${CMAKE_MATCH_1})
                list(APPEND node_sizes ${CMAKE_MATCH_2})
            endif()
        else()
            message(
                FATAL_ERROR
                "Unable to determine node size of C++ container ${container} holding type ${type} - no error text matching node_size_of<##, ##, true> in compiler output |${nodesize_output}|"
            )
        endif()
    endforeach()

    # Return output to caller
    set(${align_result_var} ${alignments} PARENT_SCOPE)
    set(${nodesize_result_var} ${node_sizes} PARENT_SCOPE)
endfunction()

# This will write the container node sizes to an output header file
# that can be used to calculate the node size of a container holding
# the specified type.
function(get_container_node_sizes outfile)
    message(STATUS "Getting container node sizes")

    # Build up the file contents in the variable NODE_SIZE_CONTENTS,
    # as requested in container_node_sizes_impl.hpp.in
    set(NODE_SIZE_CONTENTS "")

    # Get the set of uniquely aligned types to work with
    unique_aligned_types(types alignments)
    _gcns_debug_message("=> alignments |${alignments}| types |${types}|")

    set(container_types
        forward_list
        list
        set
        multiset
        unordered_set
        unordered_multiset
        map
        multimap
        unordered_map
        unordered_multimap
        shared_ptr_stateless
        shared_ptr_stateful
    )

    foreach(container IN LISTS container_types)
        string(TOUPPER "${container}_container" container_macro_name)
        get_node_sizes_of("${container_macro_name}" "${types}" alignments node_sizes)
        _gcns_debug_message("node size of |${container_macro_name}| holding types |${types}| : alignments |${alignments}| node sizes |${node_sizes}|")

        # Generate the contents for this container type
        string(
            APPEND
            NODE_SIZE_CONTENTS
            "\

namespace detail
{
    template <std::size_t Alignment>
    struct ${container}_node_size;
"
        )

        list(LENGTH alignments n_alignments)
        math(EXPR last_alignment "${n_alignments}-1")
        foreach(index RANGE ${last_alignment})
            list(GET alignments ${index} alignment)
            list(GET node_sizes ${index} node_size)

            # Generate content for this alignment/node size in this container
            string(
                APPEND
                NODE_SIZE_CONTENTS
                "\

    template <>
    struct ${container}_node_size<${alignment}>
    : std::integral_constant<std::size_t, ${node_size}>
    {};
"
            )
        endforeach()

        # End contents for this container type
        string(
            APPEND
            NODE_SIZE_CONTENTS
            "\
} // namespace detail

template <typename T>
struct ${container}_node_size
: std::integral_constant<std::size_t,
       detail::${container}_node_size<alignof(T)>::value + sizeof(T)>
{};
"
        )
    endforeach()

    # Finally, write the file.  As a reminder, configure_file() will
    # substitute in any CMake variables wrapped in @VAR@ in the inpout
    # file and write them to the output file; and will only rewrite
    # the file and update its timestamp if the contents have changed.
    # The only variable that will be substituted is NODE_SIZE_CONTENTS
    configure_file("${_THIS_MODULE_DIR}/container_node_sizes_impl.hpp.in" ${outfile})
endfunction()

if(NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/container_node_sizes_impl.hpp)
    get_container_node_sizes(${CMAKE_CURRENT_BINARY_DIR}/container_node_sizes_impl.hpp)
endif()
