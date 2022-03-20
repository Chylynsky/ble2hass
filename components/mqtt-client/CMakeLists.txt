idf_component_register(
    SRCS
        "client.cpp"
    INCLUDE_DIRS 
        "include"
    REQUIRES
        "mqtt"
        "event"
        "utils")

set(REQUIRED_LIBS
    fmt::fmt
    expected
    sml)

target_link_libraries(${COMPONENT_LIB} PUBLIC ${REQUIRED_LIBS})
