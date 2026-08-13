# 1. Java JNI Evaluation

This directory contains the optional JNI-based evaluation target for the Java backend path.

## 1.1. Contents

- `test-run-jni.cpp`: Boost.Test executable that exercises the JNI bridge.
- `WebApp.java` / `WebApp.class`: small Java test application used by the JNI test.
- `json-java.jar`: bundled JSON dependency required by the Java sample.
- `CMakeLists.txt`: resolves Java / JNI packages and builds `test-run-jni`.
