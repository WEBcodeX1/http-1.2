# 1. HTTP Generator Unit Tests

This directory contains unit coverage for the HTTP response / message generation helpers.

## 1.1. Contents

- `test-generator-responses.cpp`: Boost.Test executable source built as `test-generator-responses`.
- `CMakeLists.txt`: links the test against the internal `httpparser` static archive, which also contains the HTTP generator helpers.
