# project settings
aux_source_directory(. SRC_LIST_TEST_BOOST_PYTHON)

# find boost::python
find_package(Boost 1.74 REQUIRED COMPONENTS system python)

# set binary executable
add_executable(test-boost-python test-boost-python.cpp)

# set include
target_include_directories(test-boost-python PRIVATE ${Python3_INCLUDE_DIRS})

# set target link flags
target_link_libraries(test-boost-python ${PYTHON_LIBRARIES})
target_link_libraries(test-boost-python ${Boost_LIBRARIES})

#target_link_libraries(test-staticfs-thread-processing ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
