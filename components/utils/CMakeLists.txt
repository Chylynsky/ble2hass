idf_component_register(
    SRCS 
        "esp_exception.cpp" 
        "json.cpp" 
        "mac.cpp" 
    INCLUDE_DIRS 
        "include"
)

set(REQUIRED_LIBS 
    fmt::fmt
    rapidjson
    span
)

target_link_libraries(${COMPONENT_LIB} PRIVATE ${REQUIRED_LIBS})
target_compile_definitions(${COMPONENT_LIB} INTERFACE B2H_LOG_LEVEL=log_level::none)
