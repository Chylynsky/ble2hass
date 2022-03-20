set(TARGET event-test)

set(LIB_SRCS)

file(GLOB SRCS "event_test.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/*_test.cpp")

set(REQUIRED_LIBS 
    Catch2::Catch2
    pthread
    expected
    fmt)

set(INCLUDE_DIRS 
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../utils/include)

add_library(${TARGET} 
    OBJECT 
    ${LIB_SRCS} 
    ${SRCS})

target_link_libraries(${TARGET} PRIVATE ${REQUIRED_LIBS})
target_include_directories(${TARGET} PRIVATE ${INCLUDE_DIRS})
