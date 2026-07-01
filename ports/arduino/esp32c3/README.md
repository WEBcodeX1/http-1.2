# ESP32-C3

As the first arduino port, the ESP32-C3 will be used for prototyping. This CMake configuration will cross-compile the httpparser as static `httpparser.a` library which afterwards can be used in your ESP-IDF project / component.

# Prerequisites

Installed and **active** ESP-IDF build environment / cross-compiler installed in `$HOME/.espressif`.

# Building

```bash
./adjust-cross-build.sh
cmake -DCMAKE_TOOLCHAIN_FILE=riscv32-cross.cmake .
make
sudo make install
```

The `make install` command will install the static library `/usr/local/lib/esp32c3/libhttpparser.a` and the c++ header files `/usr/local/include/esp32c3/`.

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
