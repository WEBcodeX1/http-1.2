add_executable(test-xml-parser-e2e test-xml-parser-e2e.cpp)

target_link_libraries(test-xml-parser-e2e ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
target_link_libraries(test-xml-parser-e2e xmlparser)
