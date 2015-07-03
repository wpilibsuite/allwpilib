file(GLOB SRCS *.cpp)

add_executable(testntcore ${SRCS})

target_link_libraries(testntcore
    ntcore
    libgmock
    libgtest
    )

