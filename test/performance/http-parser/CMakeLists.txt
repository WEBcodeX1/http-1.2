# HTTP parser performance and memory benchmark tests

# Performance test: wall-clock timing comparison (new vs legacy parsing)
add_executable(test-parser-performance test-parser-performance.cpp)
target_link_libraries(test-parser-performance httpparser)

# Memory test: heap-allocation comparison (new vs legacy parsing)
add_executable(test-parser-memory test-parser-memory.cpp)
target_link_libraries(test-parser-memory httpparser)
