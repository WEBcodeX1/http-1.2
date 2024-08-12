# add source dir
aux_source_directory(. SRC_LIST_TEST_STRING_FUNCTIONS)

add_executable(test-string-functions "test-string-functions.cpp")

target_link_libraries(test-string-functions ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
