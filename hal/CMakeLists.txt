project(hal)


# Java bindings
if (NOT WITHOUT_JAVA)
    find_package(Java REQUIRED)
    find_package(JNI REQUIRED)
    include(UseJava)
    set(CMAKE_JAVA_COMPILE_FLAGS "-Xlint:unchecked")

    file(GLOB
        hal_shared_jni_src src/main/native/cpp/jni/*.cpp
        hal_sim_jni_src src/main/native/sim/jni/*.cpp)

    file(GLOB_RECURSE JAVA_SOURCES src/main/java/*.java)
    set(CMAKE_JNI_TARGET true)

    if(${CMAKE_VERSION} VERSION_LESS "3.11.0")
        set(CMAKE_JAVA_COMPILE_FLAGS "-h" "${CMAKE_CURRENT_BINARY_DIR}/jniheaders")
        add_jar(hal_jar ${JAVA_SOURCES} INCLUDE_JARS wpiutil_jar OUTPUT_NAME wpiHal)
    else()
        add_jar(hal_jar ${JAVA_SOURCES} INCLUDE_JARS wpiutil_jar OUTPUT_NAME wpiHal GENERATE_NATIVE_HEADERS hal_jni_headers)
    endif()

    get_property(HAL_JAR_FILE TARGET hal_jar PROPERTY JAR_FILE)
    install(FILES ${HAL_JAR_FILE} DESTINATION "${java_lib_dest}")

    set_property(TARGET hal_jar PROPERTY FOLDER "java")

endif()

file(GLOB
    hal_shared_native_src src/main/native/cpp/cpp/*.cpp
    hal_shared_native_src src/main/native/cpp/handles/*.cpp
    hal_mau_native_src src/main/native/mau/*.cpp
    hal_mau_native_src src/main/native/mau/Translator/*.cpp
    hal_mau_native_src src/main/native/mau/DriverStation/*.cpp)
add_library(hal_mau ${hal_shared_native_src} ${hal_shared_jni_src})

if(USE_EXTERNAL_HAL)
    include(${EXTERNAL_HAL_FILE})
else()
    target_sources(hal_mau PRIVATE ${hal_mau_native_src})
endif()

set_target_properties(hal_mau PROPERTIES OUTPUT_NAME "wpiHalMau")

target_include_directories(hal_mau PUBLIC
                $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/include>
                            $<INSTALL_INTERFACE:${include_dest}/hal>)
target_include_directories(hal_mau PUBLIC /usr/local/include/vmxpi)
target_include_directories(hal_mau PUBLIC 
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/include/HAL/handles>)
target_include_directories(hal_mau PUBLIC 
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/include/HAL>)
target_include_directories(hal_mau PUBLIC 
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/mau>)


target_link_libraries(hal_mau PUBLIC wpiutil)


set_property(TARGET hal_mau PROPERTY FOLDER "librariesmau")

if (NOT WITHOUT_JAVA)
    if(${CMAKE_VERSION} VERSION_LESS "3.11.0")
        target_include_directories(hal PRIVATE ${JNI_INCLUDE_DIRS})
        target_include_directories(hal PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/jniheaders")
    else()
        target_link_libraries(hal PRIVATE hal_jni_headers)
    endif()
    add_dependencies(hal hal_jar)
endif()

install(TARGETS hal_mau EXPORT hal_mau DESTINATION "${main_lib_dest}")
install(DIRECTORY src/main/native/include/ DESTINATION "/usr/local/wpilib/include/hal_mau")

if (NOT WITHOUT_JAVA AND MSVC)
    install(TARGETS hal RUNTIME DESTINATION "${jni_lib_dest}" COMPONENT Runtime)
endif()

if (MSVC)
    set (hal_config_dir ${wpilib_dest})
else()
	set (hal_mau_config_dir share/hal_mau)
endif()


install(FILES hal_mau-config.cmake DESTINATION ${hal_mau_config_dir})
install(EXPORT hal_mau DESTINATION ${hal_mau_config_dir})

