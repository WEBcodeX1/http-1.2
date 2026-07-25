add_executable(test-xml-parser-performance test-xml-parser-performance.cpp)

target_link_libraries(test-xml-parser-performance ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
target_link_libraries(test-xml-parser-performance xmlparser)
