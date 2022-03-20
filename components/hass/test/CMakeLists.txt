set(TARGET hass-test)

set(LIB_SRCS)

file(GLOB SRCS "hass_test.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/*_test.cpp")

set(REQUIRED_LIBS
    span
    rapidjson
    Catch2::Catch2)

set(INCLUDE_DIRS 
    ${CMAKE_CURRENT_SOURCE_DIR}/../include)

add_library(${TARGET} 
    OBJECT 
    ${LIB_SRCS} 
    ${SRCS}
    ${CMAKE_CURRENT_SOURCE_DIR}/../device_types.cpp)

target_link_libraries(${TARGET} PRIVATE ${REQUIRED_LIBS})
target_include_directories(${TARGET} PRIVATE ${INCLUDE_DIRS})
