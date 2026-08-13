# 1. HTTP Parser Benchmarks

This directory contains benchmark targets and stored result snapshots for the internal HTTP parser.

## 1.1. Contents

- `test-parser-performance.cpp`: wall-clock performance benchmark.
- `test-parser-memory.cpp`: heap-allocation / memory benchmark.
- `results-performance.csv`: recorded parser timing results.
- `results-memory.csv`: recorded parser memory results.
- `CMakeLists.txt`: builds the benchmark executables against `httpparser`.
