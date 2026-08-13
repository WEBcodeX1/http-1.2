# 1. Application Server Backend Headers

This directory contains the backend-specific interface headers used by the application-server process layer.

## 1.1. Contents

- `PythonBackend.hpp`: Python embedding interface used by the default build.
- `JavaBackend.hpp`: JNI-based backend interface enabled when the project is configured with `-DJAVA_BACKEND=1`.
