# project settings
cmake_minimum_required(VERSION 3.11)
project(esp32s3-httplib)

set(CMAKE_CXX_FLAGS "-g -fPIC -Wall -Os -std=c++11 -Wno-unused-result -Wsign-compare -Wformat -Werror=format-security -DNDEBUG -fwrapv")

# add source dir
aux_source_directory(../../../lib/http/ SRC_LIST_LIBHTTP)

# add library
add_library(httpparser STATIC ${SRC_LIST_LIBHTTP})

# add c++ header files
add_custom_target(
    httpparserheader
    SOURCES
    ../../../lib/http/httpparser.hpp
    ../../../lib/http/httpgenerator.hpp
)

# install
install(
    TARGETS httpparser
    ARCHIVE DESTINATION /usr/local/lib/esp32s3
)

# install header file
install(
    FILES
    ../../../lib/http/httpparser.hpp
    ../../../lib/http/httpgenerator.hpp
    DESTINATION /usr/local/include/esp32s3
)
