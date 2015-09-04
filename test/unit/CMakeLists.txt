file(GLOB SRCS *.cpp)

add_executable(testntcore ${SRCS})
add_test(testntcore testntcore)

if(MSVC)
target_link_libraries(testntcore
    ntcore_static
    gmock
    gtest
    )
else()
target_link_libraries(testntcore
    ntcore
    gmock
    gtest
    )
endif()
