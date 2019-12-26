cmake_minimum_required(VERSION 3.10)

project(utils)

set(TEST_SOURCES
    ${CMAKE_SOURCE_DIR}/src/countdownlatch.c
    ${CMAKE_SOURCE_DIR}/test/test_countdownlatch.c
)

add_executable(test ${TEST_SOURCES})

target_include_directories(
    test PUBLIC ${PROJECT_SOURCE_DIR}/src
)

target_link_libraries(
    test PRIVATE pthread
)
