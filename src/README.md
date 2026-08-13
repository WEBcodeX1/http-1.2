# 1. FalconAS Server Sources

This directory contains the main application sources that build the `falcon-as` executable.

## 1.1. Main Components

- `main.cpp`: entry point that constructs and starts the server.
- `Server.*`: socket setup, accept loop, shared-memory setup, and process termination orchestration.
- `Client*` and `ClientHandler*`: client connection tracking, epoll integration, and request processing.
- `Configuration*`: JSON configuration loading and namespace / MIME type setup.
- `Filesystem*`: static file namespace helpers.
- `ASProcessHandler*`: application-server child process handling for Python or Java backends.
- `CPU*`, `MemoryManager.hpp`, `SHMVector.hpp`, `Vector.hpp`: low-level runtime helpers.
- [`ASBackend/`](./ASBackend/): backend-specific interface headers.
