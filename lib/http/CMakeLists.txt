# add source dir
aux_source_directory(. SRC_LIST_LIBHTTP)

# add library
add_library(httpparser STATIC ${SRC_LIST_LIBHTTP})

# add lib parser and network
add_custom_target(
    httpparserheader
    SOURCES
    ./httpparser.hpp
)

# install static .a library
install(
    TARGETS httpparser
    ARCHIVE DESTINATION /usr/local/lib
)

# install header file
install(
    FILES
    ./httpparser.hpp
    DESTINATION /usr/local/include
)
