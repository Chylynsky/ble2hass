idf_component_register(
    SRCS
        "device_types.cpp"
    INCLUDE_DIRS 
        "include")

set(REQUIRED_LIBS
    span
    rapidjson)

target_link_libraries(${COMPONENT_LIB} PRIVATE ${REQUIRED_LIBS})
