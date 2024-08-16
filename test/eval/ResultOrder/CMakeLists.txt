# project settings
aux_source_directory(. SRC_LIST_RESULT_ORDER)

# find boost::python
find_package(Boost 1.74 REQUIRED COMPONENTS system python)

# set binary executable
add_executable(test-result-order test-result-order.cpp)

# link unit test framework
target_link_libraries(test-result-order ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

# add custom targets
add_custom_target(
    test-result-order-header
    SOURCES
    ./test-result-order.hpp
)
