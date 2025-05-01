# add source dir
aux_source_directory(. SRC_LIST_TEST_HTTPPARSER)

add_executable(
    test-parser-requests
    "test-parser-requests.cpp"
    ../../../Client.cpp
    ../../../IPCHandler.cpp
    ../../../IPCHandlerAS.cpp
    ../../../ASRequestHandler.cpp
)

# add custom targets
add_custom_target(
    TestParserHeader
    SOURCES
    ../../../lib/http/httpparser.hpp
)

# link libraries
target_link_libraries(test-parser-requests ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
target_link_libraries(test-parser-requests ${Boost_LIBRARIES})
target_link_libraries(test-parser-requests ${PYTHON_LIBRARIES})
target_link_libraries(test-parser-requests libhttp)
