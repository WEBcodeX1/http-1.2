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

In addition to the HTTP parser, the library also contains an HTTP response generator. Its purpose is to build valid and consistent HTTP/1.1 responses in a structured way, so that application code does not need to assemble response messages manually.

The response generator should make it easier to produce standards-compliant responses, reduce duplicated formatting logic, and improve maintainability for both embedded and external use cases. The following sub-topics describe the planned responsibilities and behavior of the response generator in more detail.

#### 2.1. Result Code

The response generator will provide a clear and reliable way to set the HTTP result code for a response, such as `200`, `404`, or `500`. It should ensure that valid status codes are used consistently and that application code can assign them without manually constructing the status line each time.

#### 2.2. Result Message

In addition to the numeric result code, the response generator will handle the associated result message, such as `OK`, `Not Found`, or `Internal Server Error`. This should allow the library to generate complete and readable HTTP status lines while keeping the mapping between result codes and messages predictable and maintainable.

#### 2.3. Date Header

The response generator will automatically support creation of the `Date` header for HTTP responses. This is important for standards compliance and interoperability with browsers and other clients. The implementation should ensure that the date is generated in the correct HTTP format and can be included consistently in every response where required.

#### 2.4. Adding Custom Headers

The response generator will provide a mechanism for adding custom headers to a response in a controlled and extensible way. This includes headers such as `Content-Type`, `Cache-Control`, or application-specific metadata. The design should make it possible to add, update, and serialize headers cleanly without mixing header construction logic directly into application code.

### Version 0.3

Version `v0.3` focuses on simplifying the current runtime architecture in `src`.
At the moment, request parsing, application-server execution, result handling, and socket output are distributed across multiple components and processes. This increases complexity in coordination, shared-memory usage, descriptor passing, and object ownership.

Version `v0.3` will:

1. Remove the dedicated result-processing pipeline
2. Move result handling back into the main server process
3. Eliminate client file-descriptor passing between processes or threads
4. Reduce shared-memory usage to application-server communication only
5. Clarify the object roles of `ClientHandler`, `Client`, and `HTTPParser`
6. Add an XML protocol parser library with queueing support
7. Replace result ordering with request UUID handling
8. Add a client request library including tests
9. Integrate parsing and encryption in a fixed-size threaded model with protected request and result queues

> [!NOTE]
> The planned `v0.3` work is based on the current source layout in `/src`, especially `Server`, `ClientHandler`, `ASRequestHandler`, `ASProcessHandler`, `ResultProcessor`, `ResultOrder`, and `ThreadHandler`.

#### 3.1. Remove Dedicated Result Processing

The current implementation uses `ResultProcessor`, `ResultOrder`, and `ThreadHandler` as separate stages for response processing. Version `v0.3` will remove this pipeline and simplify response generation and delivery.

#### 3.2. Move Result Handling into the Main Server Process

Result processing should no longer run in a separate process model. Instead, the main server flow should remain responsible for coordinating request completion and emitting final responses.

#### 3.3. Remove File-Descriptor Passing

The current architecture requires passing `clientFD` values between processes. Version `v0.3` should eliminate this mechanism so that the component writing the response already owns the client connection.

#### 3.4. Reduce Shared-Memory Usage

Shared memory should no longer be used for the full request/response lifecycle. It should be limited to backend or application-server communication where cross-process exchange is still necessary.

#### 3.5. Clarify Client and Parser Responsibilities

The current relationships between `ClientHandler`, `Client`, and `HTTPParser` are mixed. Version `v0.3` will establish clearer ownership between connection state, request state, and parsing logic.

#### 3.6. Add an XML Protocol Parser Library

An additional XML protocol parser library will be introduced following the same principles as the refactored HTTP parser. It should provide structured results, queueing support, and efficient transfer using move semantics.

#### 3.7. Replace Result Ordering with Request UUIDs

The current result model depends on explicit ordering logic such as `ReqNr` and `_LastRequest`. Version `v0.3` will replace this approach with request UUID handling.

#### 3.8. Add a Client Request Library

In addition to the protocol parsing library, a client request library with tests will be added for generating and sending requests in external projects.

#### 3.9. Add a Fixed-Size Threaded Processing Model

Version `v0.3` will introduce a controlled threaded model with a fixed number of worker threads, explicit request and result queues, atomic synchronization, and integrated parsing and encryption stages.

## Future

The following items are planned for later stages after the core `v0.3` refactoring is completed:

- Adapt static file transmission into the **new** NLAFP component
- Add NETCONF integration
