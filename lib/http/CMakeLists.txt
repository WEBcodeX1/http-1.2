# find boost::python
find_package(Boost 1.74 REQUIRED)

# add source dir
aux_source_directory(. SRC_LIST_LIBHTTP)

# add library
add_library(httpparser STATIC ${SRC_LIST_LIBHTTP})

# add lib parser and network
add_custom_target(
    httpparserheader
    SOURCES
    ./httpparser.hpp
    ./httpnet.hpp
)

# set target link libraries
target_link_libraries(httpparser ${Boost_LIBRARIES})

# install
install(TARGETS httpparser DESTINATION lib)
