# add source dir
aux_source_directory(. SRC_LIST_LIBEVENT)

# add library
add_library(libevent2 SHARED ${SRC_LIST_LIBEVENT})

# add header
add_custom_target(LibEventHeader SOURCES ./Event.hpp)

# install
install(TARGETS libevent2 DESTINATION lib)
