project(wpiutil)

include(SubDirList)
include(GenResources)

# Java bindings
if (NOT WITHOUT_JAVA)
  find_package(Java)
  include(UseJava)
  set(CMAKE_JAVA_COMPILE_FLAGS "-Xlint:unchecked")

  set(CMAKE_JAVA_INCLUDE_PATH wpiutil.jar)

  file(GLOB_RECURSE JAVA_SOURCES src/main/java/*.java)
  add_jar(wpiutil_jar ${JAVA_SOURCES} OUTPUT_NAME wpiutil)

  get_property(WPIUTIL_JAR_FILE TARGET wpiutil_jar PROPERTY JAR_FILE)
  install(FILES ${WPIUTIL_JAR_FILE} DESTINATION "${java_lib_dest}")

  set_property(TARGET wpiutil_jar PROPERTY FOLDER "java")

endif()

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

GENERATE_RESOURCES(src/main/native/resources generated/main/cpp WPI wpi wpiutil_resources_src)

file(GLOB_RECURSE wpiutil_native_src src/main/native/cpp/*.cpp)

file(GLOB uv_native_src src/main/native/libuv/*.cpp)

file(GLOB uv_windows_src src/main/native/libuv/win/*.cpp)

set(uv_unix_src
    src/main/native/libuv/unix/async.cpp
    src/main/native/libuv/unix/core.cpp
    src/main/native/libuv/unix/dl.cpp
    src/main/native/libuv/unix/fs.cpp
    src/main/native/libuv/unix/getaddrinfo.cpp
    src/main/native/libuv/unix/getnameinfo.cpp
    src/main/native/libuv/unix/loop-watcher.cpp
    src/main/native/libuv/unix/loop.cpp
    src/main/native/libuv/unix/pipe.cpp
    src/main/native/libuv/unix/poll.cpp
    src/main/native/libuv/unix/process.cpp
    src/main/native/libuv/unix/signal.cpp
    src/main/native/libuv/unix/stream.cpp
    src/main/native/libuv/unix/tcp.cpp
    src/main/native/libuv/unix/thread.cpp
    src/main/native/libuv/unix/timer.cpp
    src/main/native/libuv/unix/tty.cpp
    src/main/native/libuv/unix/udp.cpp
)

set(uv_darwin_src
    src/main/native/libuv/unix/bsd-ifaddrs.cpp
    src/main/native/libuv/unix/darwin.cpp
    src/main/native/libuv/unix/darwin-proctitle.cpp
    src/main/native/libuv/unix/fsevents.cpp
    src/main/native/libuv/unix/kqueue.cpp
    src/main/native/libuv/unix/proctitle.cpp
)

set(uv_linux_src
    src/main/native/libuv/unix/linux-core.cpp
    src/main/native/libuv/unix/linux-inotify.cpp
    src/main/native/libuv/unix/linux-syscalls.cpp
    src/main/native/libuv/unix/procfs-exepath.cpp
    src/main/native/libuv/unix/proctitle.cpp
    src/main/native/libuv/unix/sysinfo-loadavg.cpp
    src/main/native/libuv/unix/sysinfo-memory.cpp
)

add_library(wpiutil ${wpiutil_native_src} ${uv_native_src} ${wpiutil_resources_src})
set_target_properties(wpiutil PROPERTIES DEBUG_POSTFIX "d")

set_property(TARGET wpiutil PROPERTY FOLDER "libraries")

if(NOT MSVC)
    target_sources(wpiutil PRIVATE ${uv_unix_src})
    if (APPLE)
        target_sources(wpiutil PRIVATE ${uv_darwin_src})
    else()
        target_sources(wpiutil PRIVATE ${uv_linux_src})
    endif()
    target_compile_options(wpiutil PUBLIC -std=c++14 -Wall -pedantic -Wextra -Wno-unused-parameter)
    target_compile_options(wpiutil PRIVATE -D_GNU_SOURCE)
else()
    target_sources(wpiutil PRIVATE ${uv_windows_src})
    target_compile_options(wpiutil PUBLIC -DNOMINMAX)
    target_compile_options(wpiutil PRIVATE -D_CRT_SECURE_NO_WARNINGS)
    if(BUILD_SHARED_LIBS)
        target_compile_options(wpiutil PRIVATE -DBUILDING_UV_SHARED)
    endif()
endif()

target_link_libraries(wpiutil Threads::Threads ${CMAKE_DL_LIBS})
target_include_directories(wpiutil PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/include>
                            $<INSTALL_INTERFACE:${include_dest}/wpiutil>)
target_include_directories(wpiutil PRIVATE
    src/main/native/libuv
    src/main/native/include/uv-private
)

install(TARGETS wpiutil EXPORT wpiutil DESTINATION "${main_lib_dest}")
install(DIRECTORY src/main/native/include/ DESTINATION "${include_dest}/wpiutil")

if (NOT WITHOUT_JAVA AND MSVC)
    install(TARGETS wpiutil RUNTIME DESTINATION "${jni_lib_dest}" COMPONENT Runtime)
endif()

if (MSVC)
    set (wpiutil_config_dir ${wpilib_dest})
else()
    set (wpiutil_config_dir share/wpiutil)
endif()

install(FILES wpiutil-config.cmake DESTINATION ${wpiutil_config_dir})
install(EXPORT wpiutil DESTINATION ${wpiutil_config_dir})

SUBDIR_LIST(wpiutil_examples "${CMAKE_CURRENT_SOURCE_DIR}/examples")
foreach(example ${wpiutil_examples})
    file(GLOB wpiutil_example_src examples/${example}/*.cpp)
    if(wpiutil_example_src)
        add_executable(wpiutil_${example} ${wpiutil_example_src})
        target_link_libraries(wpiutil_${example} wpiutil)
    endif()
endforeach()

if (UNIX AND NOT APPLE)
    set (LIBUTIL -lutil)
else()
    set (LIBUTIL)
endif()
file(GLOB netconsoleServer_src src/netconsoleServer/native/cpp/*.cpp)
add_executable(netconsoleServer ${netconsoleServer_src})
target_link_libraries(netconsoleServer wpiutil ${LIBUTIL})

file(GLOB netconsoleTee_src src/netconsoleTee/native/cpp/*.cpp)
add_executable(netconsoleTee ${netconsoleTee_src})
target_link_libraries(netconsoleTee wpiutil)
