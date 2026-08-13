# 1. HTTP Parser / Generator Library

This directory contains the internal static library `httpparser` that is linked into the server, unit tests, and benchmark targets.

## 1.1. Contents

- `httpparser.cpp` / `httpparser.hpp`: default C++23 parser implementation using `std::string_view`, `std::span`, and heterogeneous lookups.
- `httpparser-cpp11.cpp` / `httpparser-cpp11.hpp`: legacy parser implementation enabled with `-DUSE_LEGACY_CPP11=ON`.
- `httpgenerator.cpp` / `httpgenerator.hpp`: HTTP response / message generation helpers.
- `httpconstants.hpp` and `httpconstants-cpp11.hpp`: parser and generator constants for the corresponding implementation.
- `CMakeLists.txt`: selects the active parser implementation and builds the static archive.
