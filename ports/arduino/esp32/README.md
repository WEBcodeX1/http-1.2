# ESP32-C3

As the first arduino port, the ESP32-C3 will be used for the prototyping. This CMake configuration will cross-compile the httpparser as static `httpparser.a` library which afterwards can be used in your ESP-IDF project / component.

# Prerequisites

Installed and **active** ESP-IDF build environment / cross-compiler installed in `$HOME/.espressif`.

# Building

```bash
./adjust-cross-build.sh
cmake -DCMAKE_TOOLCHAIN_FILE=riscv32-cross.cmake .
make
make install
```

The `make install` command will install the static library `/usr/local/lib/httpparser.a` and the c++ header file `/usr/local/include/httpparser.hpp`.
