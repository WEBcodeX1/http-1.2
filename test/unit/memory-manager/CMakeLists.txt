# test memory manager
add_executable(test-memory-manager test-memory-manager.cpp)
target_link_libraries(test-memory-manager ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
add_test(NAME test-memory-manager COMMAND test-memory-manager)
