idf_component_register(
    SRCS
        "ble.cpp"
        "central.cpp"
        "client.cpp"
    INCLUDE_DIRS 
        "include"
    REQUIRES
        "bt"
        "event"
        "utils")

set(REQUIRED_LIBS
    fmt::fmt
    expected
    span
    sml)

target_link_libraries(${COMPONENT_LIB} PRIVATE ${REQUIRED_LIBS})
