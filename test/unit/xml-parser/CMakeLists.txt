add_executable(test-xml-parser test-xml-parser.cpp)
add_executable(test-xml-parser-fuzz test-xml-parser-fuzz.cpp)

add_custom_target(
    TestXMLParserHeader
    SOURCES
    ../../../lib/xml/xmlparser.hpp
)

target_link_libraries(test-xml-parser ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
target_link_libraries(test-xml-parser xmlparser)

target_link_libraries(test-xml-parser-fuzz ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
target_link_libraries(test-xml-parser-fuzz xmlparser)
