idf_component_register(
    SRCS
        "base.cpp"
    INCLUDE_DIRS 
        "include"
    REQUIRES
        "ble"
        "mqtt-client"
        "hass"
        "utils")

set(REQUIRED_LIBS
    fmt::fmt
    rapidjson
    span
)

target_link_libraries(${COMPONENT_LIB} PUBLIC ${REQUIRED_LIBS})
