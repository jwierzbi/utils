# Copyright (c) 2020, Jarosław Tomasz Wierzbicki
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
# 
# 1. Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.

cmake_minimum_required(VERSION 3.10)

project(utils)

include(ExternalProject)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -pedantic -std=c11 -fsanitize=address,pointer-compare,pointer-subtract,leak,undefined")
set(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=undefined")

externalproject_add(
    libgtest

    TIMEOUT 30

    URL https://github.com/google/googletest/archive/release-1.7.0.tar.gz

    CMAKE_ARGS ${CMAKE_ARGS}

    INSTALL_DIR "${CMAKE_BINARY_DIR}/external"
    INSTALL_COMMAND sh -c "install -d <INSTALL_DIR>/include/gtest/internal <INSTALL_DIR>/lib"
            COMMAND sh -c "install -m 644 <SOURCE_DIR>/include/gtest/*.h <INSTALL_DIR>/include/gtest/"
            COMMAND sh -c "install -m 644 <SOURCE_DIR>/include/gtest/internal/*.h <INSTALL_DIR>/include/gtest/internal/"
            COMMAND sh -c "install -m 644 <BINARY_DIR>/libgtest.a <INSTALL_DIR>/lib/"
            COMMAND sh -c "install -m 644 <BINARY_DIR>/libgtest_main.a <INSTALL_DIR>/lib/"
    BUILD_IN_SOURCE 1
)
set_target_properties(libgtest PROPERTIES EXCLUDE_FROM_ALL TRUE)

externalproject_add(
    libcmocka

    INSTALL_DIR "${CMAKE_BINARY_DIR}/external"
    URL https://git.cryptomilk.org/projects/cmocka.git/snapshot/cmocka-1.1.5.tar.xz
    TIMEOUT 30

    CMAKE_ARGS ${CMAKE_ARGS} -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
)
set_target_properties(libcmocka PROPERTIES EXCLUDE_FROM_ALL TRUE)

set(EXTERNAL_DIR ${CMAKE_BINARY_DIR}/external)
set(GTEST_STATIC_LIB ${EXTERNAL_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}gtest${CMAKE_STATIC_LIBRARY_SUFFIX})
set(GTEST_MAIN_STATIC_LIB ${EXTERNAL_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}gtest_main${CMAKE_STATIC_LIBRARY_SUFFIX})
set(CMOCKA_LIB ${EXTERNAL_DIR}/lib/libcmocka.so)

# ----- vector -----------------------------------------------------------------

set(TEST_VECTOR_SOURCES
    ${CMAKE_SOURCE_DIR}/src/vector.c
    ${CMAKE_SOURCE_DIR}/test/test_vector.c
)

add_executable(test_vector ${TEST_VECTOR_SOURCES})

target_include_directories(
    test_vector PUBLIC ${PROJECT_SOURCE_DIR}/src
    test_vector PUBLIC ${CMAKE_BINARY_DIR}/external/include
)


target_link_libraries(test_vector PRIVATE
  asan
  ${CMOCKA_LIB}
)

# ----- countdownlatch ---------------------------------------------------------

set(TEST_COUNTDOWNLATCH_SOURCES
    ${CMAKE_SOURCE_DIR}/src/countdownlatch.c
    ${CMAKE_SOURCE_DIR}/test/test_countdownlatch.c
)

add_executable(test_countdownlatch ${TEST_COUNTDOWNLATCH_SOURCES})

target_include_directories(
    test_countdownlatch PUBLIC ${PROJECT_SOURCE_DIR}/src
)

target_link_libraries(
    test_countdownlatch PRIVATE
    asan
    pthread
)

# ----- bipbuffer --------------------------------------------------------------

set(TEST_BIPBUFFER_SOURCES
    ${CMAKE_SOURCE_DIR}/src/bipbuffer.c
    ${CMAKE_SOURCE_DIR}/test/test_bipbuffer.cpp
)

add_executable(test_bipbuffer ${TEST_BIPBUFFER_SOURCES})
add_dependencies(test_bipbuffer libgtest)

target_include_directories(
    test_bipbuffer PUBLIC ${PROJECT_SOURCE_DIR}/src
    test_bipbuffer PUBLIC ${CMAKE_BINARY_DIR}/external/include
)

target_link_libraries(test_bipbuffer PRIVATE
  asan
  ${GTEST_STATIC_LIB}
  ${GTEST_MAIN_STATIC_LIB}
  pthread
)

# ----- binary_search ----------------------------------------------------------

set(TEST_BINARY_SEARCH_SOURCES
    ${CMAKE_SOURCE_DIR}/src/binary_search.c
    ${CMAKE_SOURCE_DIR}/test/test_binary_search.c
)

add_executable(test_binary_search ${TEST_BINARY_SEARCH_SOURCES})
add_dependencies(test_binary_search libcmocka)

target_include_directories(
    test_binary_search PUBLIC ${PROJECT_SOURCE_DIR}/src
    test_binary_search PUBLIC ${CMAKE_BINARY_DIR}/external/include
)

target_link_libraries(test_binary_search PRIVATE
  asan
  ${CMOCKA_LIB}
)
