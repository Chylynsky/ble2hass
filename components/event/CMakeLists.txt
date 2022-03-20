idf_component_register(
    INCLUDE_DIRS 
        "include"
    REQUIRES
        "utils")

set(REQUIRED_LIBS 
    expected)

target_link_libraries(${COMPONENT_LIB} INTERFACE ${REQUIRED_LIBS})
