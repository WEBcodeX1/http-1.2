# 1. HTTP Parser Unit Tests

This directory contains focused unit coverage for the internal HTTP request parser.

## 1.1. Contents

- `test-parser-requests.cpp`: Boost.Test executable source built as `test-parser-requests`.
- `as-get.json`: sample request / payload fixture used by the test set.
- `CMakeLists.txt`: links the test against the internal `httpparser` library.
