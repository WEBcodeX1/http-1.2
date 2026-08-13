# 1. Internal C++ Libraries

This directory contains the reusable libraries that are built before the main `falcon-as` server and linked into tests.

## 1.1. Subdirectories

- [`event/`](./event/): small shared callback / event helper library (`libevent2`).
- [`http/`](./http/): static HTTP parsing and message generation library (`httpparser`).
- [`xml/`](./xml/): static Xerces-C++ backed XML parsing library (`xmlparser`).
