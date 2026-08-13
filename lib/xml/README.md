# 1. XML Parser Library

This directory contains the internal static library `xmlparser` used for XML validation and message parsing.

## 1.1. Contents

- `xmlparser.hpp` / `xmlparser.cpp`: parser interface and implementation.
- `xmlconstants.hpp`: XML-related constants used by the parser.
- `static-dtd.hpp`: embedded DTD support used by the parser layer.
- `CMakeLists.txt`: resolves the Xerces-C++ dependency and builds the static archive.
