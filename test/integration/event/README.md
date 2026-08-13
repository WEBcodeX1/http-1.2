# 1. Event Library Integration Test

This directory contains the integration test that links against the internal `libevent2` shared library.

## 1.1. Contents

- `test-Event.cpp`: Boost.Test executable source built as `test-event`.
- `TestClass.cpp` / `TestClass.hpp`: helper class used by the event test.
- `CMakeLists.txt`: links the test against both Boost.Test and `libevent2`.
