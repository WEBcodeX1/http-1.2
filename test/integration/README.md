# 1. Integration Tests

This directory contains tests that exercise multiple components together.

## 1.1. CMake-Managed Targets

- [`event/`](./event/): validates the internal event library in a linked executable.
- [`string-functions/`](./string-functions/): checks shared string helper behavior.
- [`vector-multi-erase/`](./vector-multi-erase/): verifies vector erase semantics.
- [`signal-termination/`](./signal-termination/): validates process shutdown and SIGTERM propagation.
- [`custom-vector/`](./custom-vector/): integration coverage for the shared-memory vector implementation.
- [`xml-parser/`](./xml-parser/): end-to-end XML parser integration coverage.

## 1.2. Manual Checks

- [`request-validation/`](./request-validation/): shell-based request validation scripts that are currently stored in the repository but not added to the integration CMake tree.
