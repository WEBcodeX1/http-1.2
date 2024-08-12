# add source dir
aux_source_directory(. SRC_LIST_TEST_INTEGRATION_VECTOR_MULTI_ERASE)

add_executable(test-Vector-multi-erase "test-Vector-multi-erase.cpp")

target_link_libraries(test-Vector-multi-erase ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
