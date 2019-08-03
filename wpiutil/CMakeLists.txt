project(wpiutil)

include(SubDirList)
include(GenResources)
include(CompileWarnings)
include(AddTest)

# Java bindings
if (NOT WITHOUT_JAVA)
  find_package(Java)
  include(UseJava)
  set(CMAKE_JAVA_COMPILE_FLAGS "-Xlint:unchecked")

  if(NOT EXISTS "${CMAKE_BINARY_DIR}/wpiutil/thirdparty/ejml-simple-0.38.jar")
      set(BASE_URL "https://search.maven.org/remotecontent?filepath=")
      set(JAR_ROOT "${CMAKE_BINARY_DIR}/wpiutil/thirdparty")

      message(STATUS "Downloading EJML jarfiles...")

      file(DOWNLOAD "${BASE_URL}org/ejml/ejml-cdense/0.38/ejml-cdense-0.38.jar"
          "${JAR_ROOT}/ejml-cdense-0.38.jar")
      file(DOWNLOAD "${BASE_URL}org/ejml/ejml-core/0.38/ejml-core-0.38.jar"
          "${JAR_ROOT}/ejml-core-0.38.jar")
      file(DOWNLOAD "${BASE_URL}org/ejml/ejml-ddense/0.38/ejml-ddense-0.38.jar"
          "${JAR_ROOT}/ejml-ddense-0.38.jar")
      file(DOWNLOAD "${BASE_URL}org/ejml/ejml-dsparse/0.38/ejml-dsparse-0.38.jar"
          "${JAR_ROOT}/ejml-dsparse-0.38.jar")
      file(DOWNLOAD "${BASE_URL}org/ejml/ejml-fdense/0.38/ejml-fdense-0.38.jar"
          "${JAR_ROOT}/ejml-fdense-0.38.jar")
      file(DOWNLOAD "${BASE_URL}org/ejml/ejml-simple/0.38/ejml-simple-0.38.jar"
          "${JAR_ROOT}/ejml-simple-0.38.jar")
      file(DOWNLOAD "${BASE_URL}org/ejml/ejml-zdense/0.38/ejml-zdense-0.38.jar"
          "${JAR_ROOT}/ejml-zdense-0.38.jar")

      message(STATUS "All files downloaded.")
  endif()

  file(GLOB EJML_JARS
      ${CMAKE_SOURCE_DIR}/wpiutil/thirdparty/*.jar)

  set(CMAKE_JAVA_INCLUDE_PATH wpiutil.jar ${EJML_JARS})

  file(GLOB_RECURSE JAVA_SOURCES src/main/java/*.java)
  add_jar(wpiutil_jar ${JAVA_SOURCES} INCLUDE_JARS ${EJML_JARS} OUTPUT_NAME wpiutil)

  get_property(WPIUTIL_JAR_FILE TARGET wpiutil_jar PROPERTY JAR_FILE)
  install(FILES ${WPIUTIL_JAR_FILE} DESTINATION "${java_lib_dest}")

  set_property(TARGET wpiutil_jar PROPERTY FOLDER "java")

endif()

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

if (NOT MSVC)
    find_library(ATOMIC NAMES atomic libatomic.so.1)
    if (ATOMIC)
        message(STATUS "Found libatomic: ${ATOMIC}")
    endif()
endif()

GENERATE_RESOURCES(src/main/native/resources generated/main/cpp WPI wpi wpiutil_resources_src)

file(GLOB_RECURSE wpiutil_native_src src/main/native/cpp/*.cpp)

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
    target_compile_options(wpiutil PUBLIC /permissive- /Zc:throwingNew)
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

target_include_directories(wpiutil PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/include>
                            $<INSTALL_INTERFACE:${include_dest}/wpiutil>)

install(TARGETS wpiutil EXPORT wpiutil DESTINATION "${main_lib_dest}")
install(DIRECTORY src/main/native/include/ DESTINATION "${include_dest}/wpiutil")

if (NOT WITHOUT_JAVA AND MSVC)
    install(TARGETS wpiutil RUNTIME DESTINATION "${jni_lib_dest}" COMPONENT Runtime)
endif()

if (MSVC OR FLAT_INSTALL_WPILIB)
    set (wpiutil_config_dir ${wpilib_dest})
else()
    set (wpiutil_config_dir share/wpiutil)
endif()

configure_file(wpiutil-config.cmake.in ${CMAKE_BINARY_DIR}/wpiutil-config.cmake )
install(FILES ${CMAKE_BINARY_DIR}/wpiutil-config.cmake DESTINATION ${wpiutil_config_dir})
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
    target_link_libraries(wpiutil_test wpiutil ${LIBUTIL} gmock_main)
endif()
