# ESP32-S3

This CMake configuration will cross-compile the httpparser as static `httpparser.a` library which afterwards can be used in your ESP-IDF project / component.

# Prerequisites

Installed and **active** ESP-IDF build environment / cross-compiler installed in `$HOME/.espressif`.

# Building

```bash
./adjust-cross-build.sh
cmake -DCMAKE_TOOLCHAIN_FILE=xtensa-cross.cmake .
make
sudo make install
```

The `make install` command will install the static library `/usr/local/lib/esp32s3/libhttpparser.a` and the c++ header files `/usr/local/include/esp32s3/`.

# Using External Library

1. The c++ header files must be included in your C++ headers

```c++
#include "/usr/local/include/esp32c3/httpparser.hpp"
#include "/usr/local/include/esp32c3/httpgenerator.hpp"
```

**and**

2.  The installed static library must be linked into your executable

```cmake
add_library(httpparser_lib STATIC IMPORTED)
set_property(TARGET httpparser_lib PROPERTY IMPORTED_LOCATION /usr/local/lib/esp32c3/libhttpparser.a)

target_link_libraries(${PROJECT_NAME} PRIVATE httpparser_lib)
```
