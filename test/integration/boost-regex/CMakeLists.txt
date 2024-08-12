# add source dir
aux_source_directory(. SRC_LIST_TEST_BOOST_REGEX)

# find boost unittest package
find_package(Boost COMPONENTS regex REQUIRED)

add_executable(test-boost-regex "test-boost-regex.cpp")

target_link_libraries(test-boost-regex ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
target_link_libraries(test-boost-regex ${Boost_LIBRARIES})
