project(libuv)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

file(GLOB libuv_native_src src/main/native/c/*.c)

add_library(uv ${libuv_native_src})

set_property(TARGET uv PROPERTY FOLDER "libraries")

if(APPLE)
    target_compile_options(wpiutil PUBLIC -Wall -pedantic -Wextra -Wno-unused-parameter -D_DARWIN_USE_64_BIT_INODE=1 -D_DARWIN_UNLIMITED_SELECT=1)
elseif(NOT MSVC)
    target_compile_options(uv PUBLIC -Wall -pedantic -Wextra -Wno-unused-parameter -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -D_GNU_SOURCE)
else()
    target_compile_options(uv PUBLIC -DNOMINMAX)
endif()

target_link_libraries(uv Threads::Threads)
target_include_directories(uv PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/include>
                            $<INSTALL_INTERFACE:${include_dest}>)

install(TARGETS uv EXPORT uv DESTINATION "${main_lib_dest}")
install(DIRECTORY src/main/native/include/ DESTINATION "${include_dest}")

if (MSVC)
    set (libuv_config_dir ${wpilib_dest})
else()
    set (libuv_config_dir share/libuv)
endif()

install(FILES libuv-config.cmake DESTINATION ${libuv_config_dir})
install(EXPORT uv DESTINATION ${libuv_config_dir})
