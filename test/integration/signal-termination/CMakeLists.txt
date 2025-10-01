# add source dir
aux_source_directory(. SRC_LIST_TEST_INTEGRATION_SIGNAL_TERMINATION)

# add binary
add_executable(test-signal-termination "test-signal-termination.cpp")

# link libraries
target_link_libraries(test-signal-termination ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
