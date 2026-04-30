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

Version `v0.3` focuses on drastically simplifying the runtime architecture that currently exists in `src`.
At the moment, request parsing, application-server execution, result collection, response ordering, and final socket output are split across multiple components and even separate processes. While this design works as an experiment for concurrency and isolation, it also introduces complexity in process coordination, shared-memory synchronization, socket / file-descriptor passing, and object ownership.

The main goal of version `v0.3` is therefore to

1. Reduce shared memory data exchange (requests and results) to the application server processes only
2. Move client socket data processing to the main process where the ClientHandler also processes client data receiving
3. Add epoll filedescriptor checking for clientFD sockets are writeable
4. Add client buffering if socket write on clientFD is not possible

> [!NOTE]
> The planned `v0.3` work is based on the current source layout in `/src`, especially `Server`, `ClientHandler`, `ASRequestHandler`, `ASProcessHandler`, `ResultProcessor`, `ResultOrder`, and `ThreadHandler`.

#### 3.1. Remove the Result Processor / Thread Handler for Result Processing Completely

The current implementation contains a dedicated `ResultProcessor` process and an additional `ThreadHandler`-based response stage.

Today, the flow is roughly:

1. `ClientHandler` accepts and reads client data
2. static-file and backend request metadata are written into shared memory
3. `ResultProcessor` wakes up and reads both static-file and application-server results
4. `ResultProcessor` forwards these results into `ResultOrder`
5. `ResultOrder` groups requests by client and forwards them into `ThreadHandler`
6. `ThreadHandler` creates `ClientThread` worker threads
7. each `ClientThread` finally writes the HTTP response to the client socket

Version `v0.3` removes `ResultProcessor`, `ResultOrder`, and `ThreadHandler` completely.

#### 3.2. Move Result Processing into the Main Server Process

Currently, result processing is performed outside the main server flow.
This is visible in `Server::init()`, where the server explicitly forks a separate result processor before entering the main event loop.

The planned `v0.3` change is to bring result handling back into the main server process, or at least into a server-owned thread that does not require a separate process model.

In practical terms, this means:

- The main event loop should remain responsible not only for accepting and parsing requests, but also for coordinating completion and final response emission
- Application-server results should be collected directly by server-owned logic instead of by a separate `ResultProcessor`
- Static-file responses should be emitted directly without being serialized through a secondary result pipeline
- Response ordering decisions should happen where request state already exists, rather than in an isolated post-processing stage

This architecture is a better match for the current code because request ownership begins in `ClientHandler` and socket ownership also logically belongs there. Moving result processing back into the main flow avoids reconstructing client context later from shared memory snapshots.

#### 3.3. Remove the Necessity of Passing `clientFD` Values Between Processes or Threads

One of the clearest indicators of architectural friction in the current code is the special file-descriptor passing mechanism.

At the moment:

- The main server owns accepted client sockets
- Child processes cannot safely use the parent-owned socket descriptor directly in the intended way
- `ResultProcessor` therefore requests the descriptor from the parent over a Unix domain socket
- Helper functions in `Global.hpp` (`createFDPassingServer`, `connectFDPassingClient`, `sendFD`, `recvFD`) exist solely to make that possible
- `ResultProcessor::_getFDFromParent()` uses this mechanism before writing a response

This entire mechanism exists because response generation happens in a different process from the one that accepted the connection.

Version `v0.3` should eliminate this need.
The component that writes the NLAP response should already be in the same process context that owns the client connection, or should receive a safe internal reference that does not require SCM_RIGHTS-based descriptor transfer.

This change should remove:

- The FD-passing Unix socket server in `Server`
- The FD-passing thread in `Server::handleFDPassingRequests()`
- The descriptor request/retry path in `ResultProcessor::_getFDFromParent()`
- The extra bookkeeping fields such as `ClientFDShared` that only exist to represent transferred descriptors

#### 3.4. Remove or Reduce Shared-Memory Handling to Application-Server Processes Only

The current design uses shared memory for multiple purposes:

- Static file request forwarding
- Application-server request metadata
- Application-server request payloads
- Application-server result payloads

`Server::setupSharedMemory()` allocates several large shared-memory segments, and both `ClientHandler` and `ResultProcessor` participate in the static-file / result pipeline through those regions.

For version `v0.3`, shared memory should no longer be the transport mechanism for the full response lifecycle.
Instead, shared memory should be limited to the minimum necessary cross-process communication.

That means:

- Static file request processing should no longer require shared-memory exchange
- Parsed request metadata that remains local to the server should stay in normal in-process objects
- Only backend-bound work should be marshalled into shared memory, if backend workers remain separate processes

In the current codebase, the most likely candidate for retention is the application-server communication layer represented by:

- `ASRequestHandler`
- `IPCHandlerAS`
- `ASProcessHandler`

Everything else should gradually stop depending on shared-memory slots and atomic flags for normal request/result handling.

#### 3.5. Fix the Object Relationships Between `ClientHandler`, `Client`, and `ProtocolParser`

The current object model is not yet clean.

Observed issues in the current source include:

- `ClientHandler` stores `shared_ptr<HTTPParser>` objects directly in its client map
- `Client` exists as a separate class, but is not clearly the central runtime owner of per-client state
- `ClientThread` privately inherits from `HTTPParser`, creating another parser-bearing client-related object
- Request numbering and client state are partially distributed across parser/client/thread abstractions
- The parser is used both as a protocol utility and as part of connection/runtime ownership

This suggests that parsing concerns and connection / session concerns are currently mixed. Version `v0.3` will establish clearer roles.

#### 3.6. Implement an XML Protocol Parser *Library*

with Result Queueing Functionality Similar to `HTTPParser`, Using C++ Move Semantics

This item appears to be forward-looking and introduces a second protocol parser library following the same architectural principles as the refactored HTTP parser.

The intent is likely not to embed XML-specific application logic into the HTTP server directly, but to provide a reusable parser module with a queueing/output model similar to the HTTP parsing pipeline.

Based on the current codebase, this should mean:

- The XML parser should be implemented as its own library component
- It should not depend on server-specific connection classes more than necessary
- It should produce structured parse results rather than direct side effects
- Parsed result objects should be moveable so that large payloads are transferred efficiently without unnecessary copying

The XML parser should follow the same separation principles planned for the HTTP parser in `v0.2`:

- Protocol parsing logic only
- No hidden memory management policy
- No application-specific execution logic
- Clear status/result-based behavior

#### 3.7. Remove Result Ordering and Implement Request UUID Handling

The current result model contains explicit request ordering machinery in `ResultOrder`.

Right now:

- Requests are grouped per client
- HTTP/1.1 responses are forced into sequential order using `ReqNr`
- HTTP/1.2 responses are treated as unordered
- `_LastRequest` tracks what can be emitted next for each client

This design reflects a transport-coupled ordering strategy.
However, once work is distributed across processes and threads, numeric per-client ordering becomes fragile and adds coordination overhead.

The `v0.3` replaces this mechanism with request UUID handling.

#### 3.8. Client Rquest Library

Additional to the *ProtocolParsing* library, a *Client Request* library (including tests) will be added to correctly send requests from a client / to include in external products.

#### 3.9. Integrate Parsing and Encryption in a Threaded Model with a Fixed Thread Count and Atomically Protected Request and Result Queues

For `v0.3`, a controlled threaded architecture is planned with:

- a fixed number of worker threads
- explicit request queues (already planned to implement in HTTPParser)
- explicit result queues
- atomic safely synchronized queue protection
- integrated parsing and encryption stages

## Future

- Adapt static file transmission into NLAFP component based on the core changes in `v0.3`
- NETCONF integration
