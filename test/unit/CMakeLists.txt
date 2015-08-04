file(GLOB SRCS *.cpp)

add_executable(testntcore ${SRCS})

if(MSVC)
target_link_libraries(testntcore
    ntcore_static
    libgmock
    libgtest
    )
else()
target_link_libraries(testntcore
    ntcore
    libgmock
    libgtest
    )
endif()
