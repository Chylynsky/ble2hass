set(TARGET mqtt-test)

set(LIB_SRCS
    "../client.cpp")

file(GLOB SRCS "client_test.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/*_test.cpp")

set(REQUIRED_LIBS 
    Catch2::Catch2 
    fmt::fmt
    expected
    sml
    pthread)

set(INCLUDE_DIRS 
    ${CMAKE_CURRENT_SOURCE_DIR}/../../event/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../utils/test/mock/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../utils/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_CURRENT_SOURCE_DIR}/mock/include)

add_library(${TARGET} 
    OBJECT 
    ${LIB_SRCS} 
    ${SRCS})

target_link_libraries(${TARGET} PRIVATE ${REQUIRED_LIBS})
target_include_directories(${TARGET} PRIVATE ${INCLUDE_DIRS})