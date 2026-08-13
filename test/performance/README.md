# 1. Performance Tests

This directory contains benchmark-oriented targets and measurement notes.

## 1.1. Contents

- `00_test-performance-nokeepalive.cpp`: root performance test source collected into the `test-performance` executable.
- [`http-parser/`](./http-parser/): parser wall-clock and memory benchmarks, including recorded CSV result snapshots.
- [`xml-parser/`](./xml-parser/): XML parser benchmark target.

The database notes used to persist benchmark runs are tracked separately in [`../../specs/database/`](../../specs/database/).
