include(FetchContent)

fetchcontent_declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG
        58d77fa8070e8cec2dc1ed015d66b454c8d78850 # 1.12.1
)

fetchcontent_getproperties(googletest)
if(NOT googletest_POPULATED)
    fetchcontent_populate(googletest)

    # Prevent overriding the parent project's compiler/linker
    # settings on Windows
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

    add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

target_compile_features(gtest PUBLIC cxx_std_20)
target_compile_features(gtest_main PUBLIC cxx_std_20)
target_compile_features(gmock PUBLIC cxx_std_20)
target_compile_features(gmock_main PUBLIC cxx_std_20)
