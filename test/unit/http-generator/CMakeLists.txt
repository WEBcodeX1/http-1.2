# add source dir
aux_source_directory(. SRC_LIST_TEST_HTTPGENERATOR)

add_executable(
    test-generator-responses
    "test-generator-responses.cpp"
)

# add custom targets
add_custom_target(
    HttpGeneratorTestHeaders
    SOURCES
    ../../../lib/http/httpgenerator.hpp
)

# link libraries
target_link_libraries(test-generator-responses ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
target_link_libraries(test-generator-responses httpparser)
