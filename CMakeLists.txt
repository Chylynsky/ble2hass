cmake_minimum_required(VERSION 3.5)

set(SUPPORTED_TARGETS       esp32)

set(EXTRA_COMPONENT_DIRS    
    ./components 
    ./components/device/xiaomi
)

set(CMAKE_CXX_STANDARD          17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_C_FLAGS "-mlongcalls")
set(CMAKE_CXX_FLAGS "-mlongcalls")

if(NOT DEFINED ENV{IDF_PATH})
    message(FATAL_ERROR "Enviroment variable \"IDF_PATH\" not set.")
endif()

include($ENV{IDF_PATH}/tools/cmake/project.cmake)  
project(ble2hass)

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

include(FetchContent)

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
    fmt
    expected
)

# Special treatment for rapidjson, span and sml

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
