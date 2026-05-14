# add source dir
aux_source_directory(. SRC_LIST_TEST_XMLPARSER)

add_executable(
    test-xml-parser
    "test-xml-parser.cpp"
)

# add custom targets
add_custom_target(
    TestXMLParserHeader
    SOURCES
    ../../../lib/xml/xmlparser.hpp
)

# set absolute path to DTD so tests can be run from any directory
target_compile_definitions(
    test-xml-parser
    PRIVATE
    NLAP_DTD_PATH="${CMAKE_SOURCE_DIR}/specs/xml/nlap.dtd"
)

# link libraries
target_link_libraries(test-xml-parser ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
target_link_libraries(test-xml-parser xmlparser)
