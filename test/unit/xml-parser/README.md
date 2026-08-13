# 1. XML Parser Unit Tests

This directory contains focused unit coverage for the internal XML parser library.

## 1.1. Contents

- `test-xml-parser.cpp`: primary Boost.Test executable for parser behavior.
- `test-xml-parser-fuzz.cpp`: fuzz-style input coverage for the parser.
- `CMakeLists.txt`: links both executables against the internal `xmlparser` library.
