# 1. Unit Tests

This directory contains focused tests for individual libraries and low-level helpers.

## 1.1. Subdirectories

- [`http-parser/`](./http-parser/): request parsing tests for the internal HTTP parser.
- [`http-generator/`](./http-generator/): response generation tests for the HTTP generator.
- [`memory-alignment/`](./memory-alignment/): checks alignment-related helper behavior.
- [`memory-manager/`](./memory-manager/): tests the custom memory manager.
- [`xml-parser/`](./xml-parser/): unit coverage for the XML parser library, including fuzz-style inputs.
