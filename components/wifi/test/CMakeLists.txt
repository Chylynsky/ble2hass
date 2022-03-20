set(TARGET wifi-test)

set(LIB_SRCS "wifi_test.cpp" "../wifi.cpp")

file(GLOB SRCS "${CMAKE_CURRENT_SOURCE_DIR}/*_test.cpp")

set(REQUIRED_LIBS 
    Catch2::Catch2)

set(INCLUDE_DIRS 
    ${CMAKE_CURRENT_SOURCE_DIR}/../include 
    ${CMAKE_CURRENT_SOURCE_DIR}/mock/include)

add_library(${TARGET} 
    OBJECT 
    ${LIB_SRCS} 
    ${SRCS})

target_link_libraries(${TARGET} PRIVATE ${REQUIRED_LIBS})
target_include_directories(${TARGET} PRIVATE ${INCLUDE_DIRS})
