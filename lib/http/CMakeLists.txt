# compile-time option: build with legacy C++11 HTTP parser instead of C++23
# Enable with: cmake -DUSE_LEGACY_CPP11=ON ...
option(USE_LEGACY_CPP11 "Build using the legacy C++11 HTTP parser implementation" OFF)

if(USE_LEGACY_CPP11)
    # C++11 sources: legacy destructive-split / no string_view implementation
    set(SRC_LIST_LIBHTTP httpparser-cpp11.cpp httpgenerator.cpp)
else()
    # C++23 sources: optimised ispanstream / string_view / transparent-map implementation
    set(SRC_LIST_LIBHTTP httpparser.cpp httpgenerator.cpp)
endif()

# add library
add_library(httpparser STATIC ${SRC_LIST_LIBHTTP})

# add lib parser and network
add_custom_target(
    httpparserheader
    SOURCES
    ./httpparser.hpp
    ./httpparser-cpp11.hpp
    ./httpgenerator.hpp
)

# install static .a library
install(
    TARGETS httpparser
    ARCHIVE DESTINATION /usr/local/lib
)

# install header files
install(
    FILES
    ./httpparser.hpp
    ./httpparser-cpp11.hpp
    ./httpgenerator.hpp
    ./httpconstants.hpp
    ./httpconstants-cpp11.hpp
    DESTINATION /usr/local/include
)
