# find boost::python
find_package(Boost 1.74 REQUIRED COMPONENTS regex)

# add source dir
aux_source_directory(. SRC_LIST_LIBHTTP)

# add library
add_library(libhttp STATIC ${SRC_LIST_LIBHTTP})

# add lib parser and network
add_custom_target(
    libhttpheader
    SOURCES
    ./httpparser.hpp
    ./httpnet.hpp
)

# set target link libraries
target_link_libraries(libhttp ${Boost_LIBRARIES})

# install
install(TARGETS libhttp DESTINATION lib)
