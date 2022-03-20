idf_component_register(
    SRCS  
        "station.cpp" 
    INCLUDE_DIRS
        "include"
    REQUIRES
        "utils"
        "event")

set(REQUIRED_LIBS
    fmt::fmt
    expected)

set(INCLUDE_DIRS include)

target_link_libraries(${COMPONENT_LIB} PRIVATE ${REQUIRED_LIBS})
target_include_directories(${COMPONENT_LIB} PRIVATE ${INCLUDE_DIRS})
