# Refactoring

This document describes the refactoring and migration process from version `v0.1` to `v0.2` and later to `v0.3`.

> [!NOTE]
> This document is a work in progress.

## Version 0.2

In version `v0.1`, the HTTP parser library is not cleanly structured. Memory handling is mixed with parser functionality, which makes the library difficult to reuse in external projects.

The goal of the refactoring is to make the library usable both for the internal Arduino microcontroller port and for other external projects.

Version `v0.2` will:

- Remove memory handling from the library completely
- Provide stable, secure, and browser-compatible processing for HTTP requests (initially only GET and POST)
- Add and update library documentation

> [!NOTE]
> Version `v0.2` is intended to provide a tested and reliable library for use in external projects.

### 1. HTTP-Parser

1. The HTTP parser should only handle HTTP/1.1 requests
2. Remove C++ exceptions and replace them with direct, explicit object status checks (`if` clauses)
3. Remove or decapsulate shared memory management functionality
4. Improve GET parameter parsing and result handling
5. Add limit checks to guarantee fail-safe operation
6. Add a request queueing and request-handling mechanism
7. Add tests and improve code quality
8. Ensure C++11 conformity (for embedded compatibility, smaller binaries, and security)

#### 1.1. HTTP/1.1 Requests Only

The parser will be restricted to HTTP/1.1 request handling only. This simplifies the implementation, reduces ambiguity, and avoids the need to support incompatible or unnecessary protocol variants for the embedded target.

#### 1.2. Remove C++ Exceptions

C++ exceptions will be removed from the parser library and replaced with explicit status reporting and conditional checks. This improves portability, makes control flow easier to follow, and is better suited for embedded environments.

#### 1.3. Memory Management

Memory management responsibilities will be removed from the parser or clearly separated from its core functionality. The parser should focus only on protocol parsing and validation so that it can be reused in other projects without depending on a specific memory model.

#### 1.4. GET Parameter Parsing

GET parameter parsing will be improved to produce clearer and more reliable results. This includes better separation of keys and values, more predictable result structures, and safer handling of malformed or incomplete query strings.

#### 1.5. Limits Checking

Additional limit checks will be introduced to guarantee fail-safe behavior. This includes checking request sizes, buffer boundaries, header counts, and other parser limits to prevent invalid input from causing unsafe behavior.

#### 1.6. Result Queuing

A request/result queueing mechanism will be added to improve how parsed requests are handed off for further processing. This is intended to make request handling more robust and to prepare the parser for use in threaded or constrained runtime environments.

#### 1.7. Tests / Code Quality

Test coverage will be expanded and the general code quality will be improved. This includes adding regression tests, validating parser edge cases, and restructuring code where necessary to make behavior easier to verify and maintain.

#### 1.8. C++11 Conformity

The refactored parser will be kept compatible with C++11. This is important for embedded toolchains and helps keep the codebase portable, efficient, and easier to integrate into constrained environments such as Arduino-based targets.

### 2. HTTP Response Generator

#### 2.1. Result Code

#### 2.2. Result Message

#### 2.3. Date Header

#### 2.4. Adding Custom Headers

## Version 0.3

- Remove the result processor / thread handler for result processing completely
- Move result processing into the main server process or thread
- This removes the necessity of passing `clientFD` values between processes or threads
- This also removes or reduces shared-memory handling to application server processes only
- Fix the object relationships between `ClientHandler`, `Client`, and `ProtocolParser`
- Implement an XML protocol parser *library* with result queueing functionality similar to `HTTPParser`, using C++ move semantics
- Remove result ordering and implement request UUID handling
- Integrate parsing and encryption in a threaded model with a fixed thread count and atomically protected request and result queues

## Future

- Based on the changes in `v0.3`, implement NLAFP `sendfile()` in a single-threaded or single-process design
- NETCONF integration
