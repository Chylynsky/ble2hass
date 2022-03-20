set(TARGET utils-test)

set(LIB_SRCS
    "../esp_exception.cpp"
    "../json.cpp"
    "../mac.cpp")

file(GLOB SRCS "json_test.cpp" "fsm_test.cpp" "esp_exception_test.cpp" "const_map_test.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/*_test.cpp")

set(REQUIRED_LIBS 
    Catch2::Catch2 
    fmt::fmt
    rapidjson
    span
)

set(INCLUDE_DIRS 
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_CURRENT_SOURCE_DIR}/mock/include)

add_library(${TARGET} 
    OBJECT 
    ${LIB_SRCS} 
    ${SRCS})

target_link_libraries(${TARGET} PRIVATE ${REQUIRED_LIBS})
target_include_directories(${TARGET} PRIVATE ${INCLUDE_DIRS})
