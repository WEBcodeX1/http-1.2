add_executable(test-CustomVector test-CustomVector.cpp)
target_link_libraries(test-CustomVector ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

add_test(NAME test-CustomVector COMMAND test-CustomVector)
