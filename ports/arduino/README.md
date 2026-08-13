# 1. Arduino / ESP-IDF Ports

This directory contains cross-build configurations for reusing the internal HTTP parser library on ESP-IDF based ESP32 targets.

## 1.1. Subdirectories

- [`esp32c3/`](./esp32c3/): RISCV32 based ESP32-C3 build configuration.
- [`esp32s3/`](./esp32s3/): Xtensa based ESP32-S3 build configuration.

Each port builds and installs a target-specific static `httpparser.a` archive together with its public headers.
