add_executable(test-SHMVector test-SHMVector.cpp)
target_link_libraries(test-SHMVector ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

add_test(NAME test-SHMVector COMMAND test-SHMVector)
