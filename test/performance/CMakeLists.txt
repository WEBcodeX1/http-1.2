# add source dir
aux_source_directory(. SRC_LIST_TEST_PERFORMANCE)

# add binary
add_executable(test-performance ${SRC_LIST_TEST_PERFORMANCE})

# link libraries
target_link_libraries(test-performance ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
