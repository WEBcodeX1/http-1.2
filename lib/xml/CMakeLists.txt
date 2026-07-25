# find Xerces-C++ (CMake built-in module available since CMake 3.9)
find_package(XercesC REQUIRED)

# add source files
aux_source_directory(. SRC_LIST_LIBXML)

# build static library
add_library(xmlparser STATIC ${SRC_LIST_LIBXML})

# include Xerces-C++ headers
target_include_directories(xmlparser PRIVATE ${XercesC_INCLUDE_DIRS})

# link against Xerces-C++
target_link_libraries(xmlparser ${XercesC_LIBRARIES})

# expose header to IDE / cmake targets
add_custom_target(
    xmlparserheader
    SOURCES
    ./xmlparser.hpp
)

# install static .a library
install(
    TARGETS xmlparser
    ARCHIVE DESTINATION /usr/local/lib
)

# install header files
install(
    FILES
    ./xmlparser.hpp
    ./xmlconstants.hpp
    ./static-dtd.hpp
    DESTINATION /usr/local/include
)
