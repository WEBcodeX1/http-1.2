# project settings
cmake_minimum_required(VERSION 3.11)
project(falcon-as-arduino)

set(CMAKE_CXX_FLAGS "-g -fPIC -Wall -O0 -std=c++11 -Wno-unused-result -Wsign-compare -Wformat -Werror=format-security -DNDEBUG -fwrapv")

# add source dir
aux_source_directory(../../../lib/http/ SRC_LIST_LIBHTTP)

# add library
add_library(httpparser STATIC ${SRC_LIST_LIBHTTP})

# add c++ header files
add_custom_target(
    httpparserheader
    SOURCES
    ../../../lib/http/httpparser.hpp
)

