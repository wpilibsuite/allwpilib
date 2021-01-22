project(wpiutil)

include(SubDirList)
include(GenResources)
include(CompileWarnings)
include(AddTest)

file(GLOB wpiutil_jni_src src/main/native/cpp/jni/WPIUtilJNI.cpp)

# Java bindings
if (WITH_JAVA)
  find_package(Java REQUIRED)
  find_package(JNI REQUIRED)
  include(UseJava)
  set(CMAKE_JAVA_COMPILE_FLAGS "-Xlint:unchecked")

  if(NOT EXISTS "${WPILIB_BINARY_DIR}/wpiutil/thirdparty/jackson/jackson-core-2.10.0.jar")
        set(BASE_URL "https://search.maven.org/remotecontent?filepath=")
        set(JAR_ROOT "${WPILIB_BINARY_DIR}/wpiutil/thirdparty/jackson")

        message(STATUS "Downloading Jackson jarfiles...")

        file(DOWNLOAD "${BASE_URL}com/fasterxml/jackson/core/jackson-core/2.10.0/jackson-core-2.10.0.jar"
            "${JAR_ROOT}/jackson-core-2.10.0.jar")
        file(DOWNLOAD "${BASE_URL}com/fasterxml/jackson/core/jackson-databind/2.10.0/jackson-databind-2.10.0.jar"
            "${JAR_ROOT}/jackson-databind-2.10.0.jar")
        file(DOWNLOAD "${BASE_URL}com/fasterxml/jackson/core/jackson-annotations/2.10.0/jackson-annotations-2.10.0.jar"
            "${JAR_ROOT}/jackson-annotations-2.10.0.jar")

        message(STATUS "All files downloaded.")
    endif()

  file(GLOB JACKSON_JARS
        ${WPILIB_BINARY_DIR}/wpiutil/thirdparty/jackson/*.jar)

  set(CMAKE_JAVA_INCLUDE_PATH wpiutil.jar ${JACKSON_JARS})

  set(CMAKE_JNI_TARGET true)

  file(GLOB_RECURSE JAVA_SOURCES src/main/java/*.java)

  if(${CMAKE_VERSION} VERSION_LESS "3.11.0")
    set(CMAKE_JAVA_COMPILE_FLAGS "-h" "${CMAKE_CURRENT_BINARY_DIR}/jniheaders")
    add_jar(wpiutil_jar ${JAVA_SOURCES} INCLUDE_JARS ${JACKSON_JARS} OUTPUT_NAME wpiutil)
  else()
    add_jar(wpiutil_jar ${JAVA_SOURCES} INCLUDE_JARS ${JACKSON_JARS} OUTPUT_NAME wpiutil GENERATE_NATIVE_HEADERS wpiutil_jni_headers)
  endif()

  get_property(WPIUTIL_JAR_FILE TARGET wpiutil_jar PROPERTY JAR_FILE)
  install(FILES ${WPIUTIL_JAR_FILE} DESTINATION "${java_lib_dest}")

  set_property(TARGET wpiutil_jar PROPERTY FOLDER "java")

  add_library(wpiutiljni ${wpiutil_jni_src})
  wpilib_target_warnings(wpiutiljni)
  target_link_libraries(wpiutiljni PUBLIC wpiutil)

  set_property(TARGET wpiutiljni PROPERTY FOLDER "libraries")

  if(${CMAKE_VERSION} VERSION_LESS "3.11.0")
    target_include_directories(wpiutiljni PRIVATE ${JNI_INCLUDE_DIRS})
    target_include_directories(wpiutiljni PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/jniheaders")
  else()
    target_link_libraries(wpiutiljni PRIVATE wpiutil_jni_headers)
  endif()
  add_dependencies(wpiutiljni wpiutil_jar)

  if (MSVC)
    install(TARGETS wpiutiljni RUNTIME DESTINATION "${jni_lib_dest}" COMPONENT Runtime)
  endif()

  install(TARGETS wpiutiljni EXPORT wpiutiljni DESTINATION "${main_lib_dest}")

endif()

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

if (NOT MSVC AND NOT APPLE)
    find_library(ATOMIC NAMES atomic libatomic.so.1)
    if (ATOMIC)
        message(STATUS "Found libatomic: ${ATOMIC}")
    endif()
endif()

GENERATE_RESOURCES(src/main/native/resources generated/main/cpp WPI wpi wpiutil_resources_src)

file(GLOB_RECURSE wpiutil_native_src src/main/native/cpp/*.cpp)
list(REMOVE_ITEM wpiutil_native_src ${wpiutil_jni_src})
file(GLOB_RECURSE wpiutil_unix_src src/main/native/unix/*.cpp)
file(GLOB_RECURSE wpiutil_windows_src src/main/native/windows/*.cpp)

file(GLOB uv_native_src src/main/native/libuv/src/*.cpp)

file(GLOB uv_windows_src src/main/native/libuv/src/win/*.cpp)

set(uv_unix_src
    src/main/native/libuv/src/unix/async.cpp
    src/main/native/libuv/src/unix/core.cpp
    src/main/native/libuv/src/unix/dl.cpp
    src/main/native/libuv/src/unix/fs.cpp
    src/main/native/libuv/src/unix/getaddrinfo.cpp
    src/main/native/libuv/src/unix/getnameinfo.cpp
    src/main/native/libuv/src/unix/loop-watcher.cpp
    src/main/native/libuv/src/unix/loop.cpp
    src/main/native/libuv/src/unix/pipe.cpp
    src/main/native/libuv/src/unix/poll.cpp
    src/main/native/libuv/src/unix/process.cpp
    src/main/native/libuv/src/unix/signal.cpp
    src/main/native/libuv/src/unix/stream.cpp
    src/main/native/libuv/src/unix/tcp.cpp
    src/main/native/libuv/src/unix/thread.cpp
    src/main/native/libuv/src/unix/tty.cpp
    src/main/native/libuv/src/unix/udp.cpp
)

set(uv_darwin_src
    src/main/native/libuv/src/unix/bsd-ifaddrs.cpp
    src/main/native/libuv/src/unix/darwin.cpp
    src/main/native/libuv/src/unix/darwin-proctitle.cpp
    src/main/native/libuv/src/unix/fsevents.cpp
    src/main/native/libuv/src/unix/kqueue.cpp
    src/main/native/libuv/src/unix/proctitle.cpp
)

set(uv_linux_src
    src/main/native/libuv/src/unix/linux-core.cpp
    src/main/native/libuv/src/unix/linux-inotify.cpp
    src/main/native/libuv/src/unix/linux-syscalls.cpp
    src/main/native/libuv/src/unix/procfs-exepath.cpp
    src/main/native/libuv/src/unix/proctitle.cpp
    src/main/native/libuv/src/unix/sysinfo-loadavg.cpp
)

add_library(wpiutil ${wpiutil_native_src} ${wpiutil_resources_src})
set_target_properties(wpiutil PROPERTIES DEBUG_POSTFIX "d")

set_property(TARGET wpiutil PROPERTY FOLDER "libraries")

target_compile_features(wpiutil PUBLIC cxx_std_17)
if (MSVC)
    target_compile_options(wpiutil PUBLIC /permissive- /Zc:throwingNew /MP /bigobj)
    target_compile_definitions(wpiutil PRIVATE -D_CRT_SECURE_NO_WARNINGS)
endif()
wpilib_target_warnings(wpiutil)
target_link_libraries(wpiutil Threads::Threads ${CMAKE_DL_LIBS} ${ATOMIC})

if (NOT USE_VCPKG_LIBUV)
    target_sources(wpiutil PRIVATE ${uv_native_src})
    install(DIRECTORY src/main/native/libuv/include/ DESTINATION "${include_dest}/wpiutil")
    target_include_directories(wpiutil PRIVATE
        src/main/native/libuv/src)
    target_include_directories(wpiutil PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/libuv/include>
                            $<INSTALL_INTERFACE:${include_dest}/wpiutil>)
    if(NOT MSVC)
        target_sources(wpiutil PRIVATE ${uv_unix_src})
        if (APPLE)
            target_sources(wpiutil PRIVATE ${uv_darwin_src})
        else()
            target_sources(wpiutil PRIVATE ${uv_linux_src})
        endif()
        target_compile_definitions(wpiutil PRIVATE -D_GNU_SOURCE)
    else()
        target_sources(wpiutil PRIVATE ${uv_windows_src})
        if(BUILD_SHARED_LIBS)
            target_compile_definitions(wpiutil PRIVATE -DBUILDING_UV_SHARED)
        endif()
    endif()
else()
    find_package(unofficial-libuv CONFIG REQUIRED)
    target_link_libraries(wpiutil unofficial::libuv::libuv)
endif()

if (MSVC)
    target_sources(wpiutil PRIVATE ${wpiutil_windows_src})
else ()
    target_sources(wpiutil PRIVATE ${wpiutil_unix_src})
endif()

target_include_directories(wpiutil PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/include>
                            $<INSTALL_INTERFACE:${include_dest}/wpiutil>)

install(TARGETS wpiutil EXPORT wpiutil DESTINATION "${main_lib_dest}")
install(DIRECTORY src/main/native/include/ DESTINATION "${include_dest}/wpiutil")

if (WITH_JAVA AND MSVC)
    install(TARGETS wpiutil RUNTIME DESTINATION "${jni_lib_dest}" COMPONENT Runtime)
endif()

if (WITH_FLAT_INSTALL)
    set (wpiutil_config_dir ${wpilib_dest})
else()
    set (wpiutil_config_dir share/wpiutil)
endif()

configure_file(wpiutil-config.cmake.in ${WPILIB_BINARY_DIR}/wpiutil-config.cmake )
install(FILES ${WPILIB_BINARY_DIR}/wpiutil-config.cmake DESTINATION ${wpiutil_config_dir})
install(EXPORT wpiutil DESTINATION ${wpiutil_config_dir})

SUBDIR_LIST(wpiutil_examples "${CMAKE_CURRENT_SOURCE_DIR}/examples")
foreach(example ${wpiutil_examples})
    file(GLOB wpiutil_example_src examples/${example}/*.cpp)
    if(wpiutil_example_src)
        add_executable(wpiutil_${example} ${wpiutil_example_src})
        wpilib_target_warnings(wpiutil_${example})
        target_link_libraries(wpiutil_${example} wpiutil)
        set_property(TARGET wpiutil_${example} PROPERTY FOLDER "examples")
    endif()
endforeach()

if (UNIX AND NOT APPLE)
    set (LIBUTIL -lutil)
else()
    set (LIBUTIL)
endif()

file(GLOB netconsoleServer_src src/netconsoleServer/native/cpp/*.cpp)
add_executable(netconsoleServer ${netconsoleServer_src})
wpilib_target_warnings(netconsoleServer)
target_link_libraries(netconsoleServer wpiutil ${LIBUTIL})

file(GLOB netconsoleTee_src src/netconsoleTee/native/cpp/*.cpp)
add_executable(netconsoleTee ${netconsoleTee_src})
wpilib_target_warnings(netconsoleTee)
target_link_libraries(netconsoleTee wpiutil)

set_property(TARGET netconsoleServer PROPERTY FOLDER "examples")
set_property(TARGET netconsoleTee PROPERTY FOLDER "examples")

if (WITH_TESTS)
    wpilib_add_test(wpiutil src/test/native/cpp)
    target_include_directories(wpiutil_test PRIVATE src/test/native/include)
    target_link_libraries(wpiutil_test wpiutil ${LIBUTIL} gmock_main)
endif()
