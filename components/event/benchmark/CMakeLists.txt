set(TARGET b2h-event-benchmark)

set(REQUIRED_LIBS 
    benchmark::benchmark_main)

set(INCLUDE_DIRS 
    ${CMAKE_CURRENT_SOURCE_DIR}/../include)

set(BENCHMARK_SRCS "event_benchmark.cpp")

add_executable(${TARGET} ${BENCHMARK_SRCS})

target_include_directories(${TARGET} PRIVATE ${INCLUDE_DIRS})
target_link_libraries(${TARGET} PRIVATE ${REQUIRED_LIBS})