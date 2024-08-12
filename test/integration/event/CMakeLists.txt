# add source dir
aux_source_directory(. SRC_LIST_TEST_INTEGRATION_EVENT)

# add binary
add_executable(test-event "test-Event.cpp" "TestClass.cpp")

# add custom targets
add_custom_target(
    TestEventHeader
    SOURCES
    ./TestClass.hpp
    ../../../lib/event/Event.hpp
)

# link libraries
target_link_libraries(test-event ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
target_link_libraries(test-event libevent2)
