# 1. Evaluation Tests

This directory contains exploratory targets used to validate implementation ideas or optional runtime integrations.

## 1.1. Contents

- `test-boost-python.cpp`: experiments with the embedded Python integration layer.
- `test-pointer-ref.cpp`: pointer / reference behavior checks.
- `test-shmem-vector.cpp`: shared-memory vector experimentation.
- [`ResultOrder/`](./ResultOrder/): focused result-ordering evaluation target.
- [`JavaJNI/`](./JavaJNI/): optional JNI evaluation target, built only when `JAVA_BACKEND` is enabled.
