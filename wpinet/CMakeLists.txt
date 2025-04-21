project(wpinet)

include(SubDirList)
include(GenResources)
include(CompileWarnings)
include(AddTest)

file(GLOB wpinet_jni_src src/main/native/cpp/jni/WPINetJNI.cpp)

# Java bindings
if(WITH_JAVA)
    include(UseJava)

    set(CMAKE_JNI_TARGET true)

    file(GLOB_RECURSE JAVA_SOURCES src/main/java/*.java)

    add_jar(
        wpinet_jar
        ${JAVA_SOURCES}
        INCLUDE_JARS wpiutil_jar
        OUTPUT_NAME wpinet
        OUTPUT_DIR ${WPILIB_BINARY_DIR}/${java_lib_dest}
        GENERATE_NATIVE_HEADERS wpinet_jni_headers
    )
    set_property(TARGET wpinet_jar PROPERTY FOLDER "java")

    install_jar(wpinet_jar DESTINATION ${java_lib_dest})
    install_jar_exports(TARGETS wpinet_jar FILE wpinet_jar.cmake DESTINATION share/wpinet)

    add_library(wpinetjni ${wpinet_jni_src})
    wpilib_target_warnings(wpinetjni)
    target_link_libraries(wpinetjni PUBLIC wpinet)

    set_property(TARGET wpinetjni PROPERTY FOLDER "libraries")

    target_link_libraries(wpinetjni PRIVATE wpinet_jni_headers)
    add_dependencies(wpinetjni wpinet_jar)

    install(TARGETS wpinetjni EXPORT wpinetjni)
    export(TARGETS wpinetjni FILE wpinetjni.cmake NAMESPACE wpinetjni::)
endif()

if(WITH_JAVA_SOURCE)
    include(UseJava)
    include(CreateSourceJar)
    add_source_jar(
        wpinet_src_jar
        BASE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/src/main/java
        OUTPUT_NAME wpinet-sources
        OUTPUT_DIR ${WPILIB_BINARY_DIR}/${java_lib_dest}
    )
    set_property(TARGET wpinet_src_jar PROPERTY FOLDER "java")

    install_jar(wpinet_src_jar DESTINATION ${java_lib_dest})
endif()

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

if(NOT MSVC AND NOT APPLE)
    find_library(ATOMIC NAMES atomic libatomic.so.1)
    if(ATOMIC)
        message(STATUS "Found libatomic: ${ATOMIC}")
    endif()
endif()

generate_resources(src/main/native/resources generated/main/cpp WPI wpi wpinet_resources_src)

file(
    GLOB_RECURSE wpinet_native_src
    src/main/native/cpp/*.cpp
    src/main/native/thirdparty/tcpsockets/cpp/*.cpp
)
list(REMOVE_ITEM wpinet_native_src ${wpinet_jni_src})
file(GLOB_RECURSE wpinet_unix_src src/main/native/unix/*.cpp)
file(GLOB_RECURSE wpinet_linux_src src/main/native/linux/*.cpp)
file(GLOB_RECURSE wpinet_macos_src src/main/native/macOS/*.cpp)
file(GLOB_RECURSE wpinet_windows_src src/main/native/windows/*.cpp)

file(GLOB uv_native_src src/main/native/thirdparty/libuv/src/*.cpp)

file(GLOB uv_windows_src src/main/native/thirdparty/libuv/src/win/*.cpp)

set(uv_unix_src
    src/main/native/thirdparty/libuv/src/unix/async.cpp
    src/main/native/thirdparty/libuv/src/unix/core.cpp
    src/main/native/thirdparty/libuv/src/unix/dl.cpp
    src/main/native/thirdparty/libuv/src/unix/fs.cpp
    src/main/native/thirdparty/libuv/src/unix/getaddrinfo.cpp
    src/main/native/thirdparty/libuv/src/unix/getnameinfo.cpp
    src/main/native/thirdparty/libuv/src/unix/loop-watcher.cpp
    src/main/native/thirdparty/libuv/src/unix/loop.cpp
    src/main/native/thirdparty/libuv/src/unix/pipe.cpp
    src/main/native/thirdparty/libuv/src/unix/poll.cpp
    src/main/native/thirdparty/libuv/src/unix/process.cpp
    src/main/native/thirdparty/libuv/src/unix/random-devurandom.cpp
    src/main/native/thirdparty/libuv/src/unix/random-getentropy.cpp
    src/main/native/thirdparty/libuv/src/unix/random-getrandom.cpp
    src/main/native/thirdparty/libuv/src/unix/signal.cpp
    src/main/native/thirdparty/libuv/src/unix/stream.cpp
    src/main/native/thirdparty/libuv/src/unix/tcp.cpp
    src/main/native/thirdparty/libuv/src/unix/thread.cpp
    src/main/native/thirdparty/libuv/src/unix/tty.cpp
    src/main/native/thirdparty/libuv/src/unix/udp.cpp
)

set(uv_darwin_src
    src/main/native/thirdparty/libuv/src/unix/bsd-ifaddrs.cpp
    src/main/native/thirdparty/libuv/src/unix/darwin.cpp
    src/main/native/thirdparty/libuv/src/unix/darwin-proctitle.cpp
    src/main/native/thirdparty/libuv/src/unix/fsevents.cpp
    src/main/native/thirdparty/libuv/src/unix/kqueue.cpp
    src/main/native/thirdparty/libuv/src/unix/proctitle.cpp
)

set(uv_linux_src
    src/main/native/thirdparty/libuv/src/unix/linux.cpp
    src/main/native/thirdparty/libuv/src/unix/procfs-exepath.cpp
    src/main/native/thirdparty/libuv/src/unix/proctitle.cpp
    src/main/native/thirdparty/libuv/src/unix/random-sysctl-linux.cpp
)

add_library(wpinet ${wpinet_native_src} ${wpinet_resources_src})
set_target_properties(wpinet PROPERTIES DEBUG_POSTFIX "d")

set_property(TARGET wpinet PROPERTY FOLDER "libraries")

target_compile_features(wpinet PUBLIC cxx_std_20)
wpilib_target_warnings(wpinet)
target_link_libraries(wpinet PUBLIC wpiutil)

if(NOT USE_SYSTEM_LIBUV)
    target_sources(wpinet PRIVATE ${uv_native_src})
    install(
        DIRECTORY src/main/native/thirdparty/libuv/include/
        DESTINATION "${include_dest}/wpinet"
    )
    target_include_directories(wpinet PRIVATE src/main/native/thirdparty/libuv/src)
    target_include_directories(
        wpinet
        PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/thirdparty/libuv/include>
    )
    if(NOT MSVC)
        target_sources(wpinet PRIVATE ${uv_unix_src})
        if(APPLE)
            target_sources(wpinet PRIVATE ${uv_darwin_src})
        else()
            target_sources(wpinet PRIVATE ${uv_linux_src})
        endif()
        target_compile_definitions(wpinet PRIVATE -D_GNU_SOURCE)
    else()
        target_sources(wpinet PRIVATE ${uv_windows_src})
        if(BUILD_SHARED_LIBS)
            target_compile_definitions(wpinet PRIVATE -DBUILDING_UV_SHARED)
        endif()
    endif()
else()
    find_package(libuv CONFIG REQUIRED)
    target_link_libraries(wpinet PUBLIC $<IF:$<TARGET_EXISTS:libuv::uv_a>,libuv::uv_a,libuv::uv>)
endif()

if(MSVC)
    target_sources(wpinet PRIVATE ${wpinet_windows_src})
else()
    target_sources(wpinet PRIVATE ${wpinet_unix_src})
    if(APPLE)
        target_sources(wpinet PRIVATE ${wpinet_macos_src})
    else()
        target_sources(wpinet PRIVATE ${wpinet_linux_src})
    endif()
endif()

install(
    DIRECTORY src/main/native/include/ src/main/native/thirdparty/tcpsockets/include/
    DESTINATION "${include_dest}/wpinet"
)
target_include_directories(
    wpinet
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/thirdparty/tcpsockets/include>
        $<INSTALL_INTERFACE:${include_dest}/wpinet>
)

install(TARGETS wpinet EXPORT wpinet)
export(TARGETS wpinet FILE wpinet.cmake NAMESPACE wpinet::)

configure_file(wpinet-config.cmake.in ${WPILIB_BINARY_DIR}/wpinet-config.cmake)
install(FILES ${WPILIB_BINARY_DIR}/wpinet-config.cmake DESTINATION share/wpinet)
install(EXPORT wpinet DESTINATION share/wpinet)

subdir_list(wpinet_examples "${CMAKE_CURRENT_SOURCE_DIR}/examples")
foreach(example ${wpinet_examples})
    file(GLOB wpinet_example_src examples/${example}/*.cpp)
    if(wpinet_example_src)
        add_executable(wpinet_${example} ${wpinet_example_src})
        wpilib_target_warnings(wpinet_${example})
        target_link_libraries(wpinet_${example} wpinet)
        set_property(TARGET wpinet_${example} PROPERTY FOLDER "examples")
    endif()
endforeach()

if(UNIX AND NOT APPLE AND NOT ANDROID)
    set(LIBUTIL -lutil)
else()
    set(LIBUTIL)
endif()

file(GLOB netconsoleServer_src src/netconsoleServer/native/cpp/*.cpp)
add_executable(netconsoleServer ${netconsoleServer_src})
wpilib_target_warnings(netconsoleServer)
target_link_libraries(netconsoleServer wpinet ${LIBUTIL})

file(GLOB netconsoleTee_src src/netconsoleTee/native/cpp/*.cpp)
add_executable(netconsoleTee ${netconsoleTee_src})
wpilib_target_warnings(netconsoleTee)
target_link_libraries(netconsoleTee wpinet)

set_property(TARGET netconsoleServer PROPERTY FOLDER "examples")
set_property(TARGET netconsoleTee PROPERTY FOLDER "examples")

if(WITH_TESTS)
    wpilib_add_test(wpinet src/test/native/cpp)
    target_include_directories(wpinet_test PRIVATE src/test/native/include src/main/native/cpp)
    target_link_libraries(wpinet_test wpinet ${LIBUTIL} googletest wpiutil_testlib)
endif()
