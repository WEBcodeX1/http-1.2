# test memory alignment
add_executable(test-memory-alignment test-memory-alignment.cpp)
target_link_libraries(test-memory-alignment ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
add_test(NAME test-memory-alignment COMMAND test-memory-alignment)
