# Copyright 2022 Borys Chyliński

# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:

# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

cmake_minimum_required(VERSION 3.5)

project(ble2hass-test LANGUAGES CXX)

include(FetchContent)
include(CTest)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(TARGET ble2hass-test)

set(SRCS "test_main.cpp")

set(REQUIRED_LIBS 
    Catch2::Catch2
    utils-test
    event-test
    hass-test
    mqtt-test)

set(PROJECT_BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../)
set(COMPONENTS_DIR ${PROJECT_BASE_DIR}/components)

# Dependency configuration

option(FMT_DOC       "Generate the doc target."                         OFF)
option(FMT_INSTALL   "Generate the install target."                     OFF)
option(FMT_TEST      "Generate the test target."                        OFF)
option(FMT_FUZZ      "Generate the fuzz target."                        OFF)
option(FMT_CUDA_TEST "Generate the cuda-test target."                   OFF)
option(FMT_OS        "Include core requiring OS (Windows/Posix) "       OFF)
option(FMT_MODULE    "Build a module instead of a traditional library." OFF)

option(RAPIDJSON_BUILD_DOC                  "Build rapidjson documentation."                                        OFF)
option(RAPIDJSON_BUILD_EXAMPLES             "Build rapidjson examples."                                             OFF)
option(RAPIDJSON_BUILD_TESTS                "Build rapidjson perftests and unittests."                              OFF)
option(RAPIDJSON_BUILD_THIRDPARTY_GTEST     "Use gtest installation in `thirdparty/gtest` by default if available"  OFF)
option(RAPIDJSON_BUILD_CXX11                "Build rapidjson with C++11"                                            OFF)
option(RAPIDJSON_BUILD_CXX17                "Build rapidjson with C++17"                                            ON)
option(RAPIDJSON_BUILD_ASAN                 "Build rapidjson with address sanitizer (gcc/clang)"                    OFF)
option(RAPIDJSON_BUILD_UBSAN                "Build rapidjson with undefined behavior sanitizer (gcc/clang)"         OFF)
option(RAPIDJSON_ENABLE_INSTRUMENTATION_OPT "Build rapidjson with -march or -mcpu options"                          OFF)
option(RAPIDJSON_HAS_STDSTRING              ""                                                                      ON)
option(RAPIDJSON_USE_MEMBERSMAP             ""                                                                      OFF)

option(BUILD_TESTING                ""  OFF)
option(EXPECTED_BUILD_PACKAGE_DEB   ""  OFF)

option(B2H_BUILD_BENCHMARK "Build ble2hass benchmarks." OFF)

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY      https://github.com/catchorg/Catch2.git
    GIT_TAG             v2.13.7
)

FetchContent_Declare(
    fmt
    GIT_REPOSITORY      https://github.com/fmtlib/fmt.git
    GIT_TAG             8.1.1
)

FetchContent_Declare(
    rapidjson
    GIT_REPOSITORY      https://github.com/Tencent/rapidjson.git
    GIT_TAG             master
)

FetchContent_Declare(
    expected
    GIT_REPOSITORY      https://github.com/TartanLlama/expected.git
    GIT_TAG             master
)

FetchContent_Declare(
    span
    GIT_REPOSITORY      https://github.com/tcbrindle/span.git
    GIT_TAG             master
)

FetchContent_Declare(
    sml
    GIT_REPOSITORY      https://github.com/boost-ext/sml.git
    GIT_TAG             v1.1.4
)

FetchContent_MakeAvailable(
    Catch2
    fmt
    expected
)

# Special treatment for rapidjson and span

FetchContent_GetProperties(rapidjson)
if(NOT rapidjson_POPULATED)
    FetchContent_Populate(rapidjson)
    add_library(rapidjson INTERFACE)
    target_compile_definitions(rapidjson INTERFACE RAPIDJSON_HAS_STDSTRING=1)
    target_compile_definitions(rapidjson INTERFACE RAPIDJSON_ALLOCATOR_DEFAULT_CHUNK_CAPACITY=64)
    target_include_directories(rapidjson INTERFACE ${rapidjson_SOURCE_DIR}/include)
endif()

FetchContent_GetProperties(span)
if(NOT span_POPULATED)
    FetchContent_Populate(span)
    add_library(span INTERFACE)
    target_include_directories(span INTERFACE ${span_SOURCE_DIR}/include)
endif()

FetchContent_GetProperties(sml)
if(NOT sml_POPULATED)
    FetchContent_Populate(sml)
    add_library(sml INTERFACE)
    target_include_directories(sml INTERFACE ${sml_SOURCE_DIR}/include)
endif()

add_subdirectory(${COMPONENTS_DIR}/utils/test utils-test-src)
add_subdirectory(${COMPONENTS_DIR}/event/test event-test-src)
add_subdirectory(${COMPONENTS_DIR}/hass/test hass-test-src)
add_subdirectory(${COMPONENTS_DIR}/mqtt-client/test mqtt-test-src)

add_executable(${TARGET} ${SRCS})

target_link_libraries(${TARGET} PRIVATE ${REQUIRED_LIBS})

enable_testing()

if(B2H_BUILD_BENCHMARK)
    set(BENCHMARK_DOWNLOAD_DEPENDENCIES ON)

    FetchContent_Declare(
        benchmark
        GIT_REPOSITORY https://github.com/google/benchmark.git
        GIT_TAG        v1.6.0
    )

    FetchContent_MakeAvailable(benchmark)

    add_subdirectory(${COMPONENTS_DIR}/event/benchmark event-benchmark-src)
endif()
