# Refactoring

This document describes the refactoring process / migration from version `v0.1` to `v0.2` to `v0.3` in detail.

> [!NOTE]
> This document is WIP.

## Version 0.2

The HTTP parser library in version 0.1 is unclean code. Memory handling is mixed with parser functionality. This prohibits the use of the library in external projects.

The goal after refactoring is to use the library for the internal arduino microcontroller port and for other projects.

Version `v0.2` will:

- Remove memory handling from the library completely
- Provide a rock solid, stable, secure and browser compatible processing (only GET and POST requests)
- Add / update library documentation

> [!NOTE]
> Version 0.2 will provide a tested, 100% working library for use in external projects.

### 1. HTTP-Parser

1. HTTP-Parser should only handle HTTP/1.1 requests
2. Remove C++ Exceptions, replace with direct (more concrete) object status checking (if clauses)
3. Remove / decapsulate Shared Memory Management functionality
4. Improve GET parameter parsing / result handling
5. Add limits checking to guarantee fail-safe operation
6. Add requests queuing / requests handling mechanism
7. Add tests / improve code quality
8. Ensure C++11 conformity (for embedded size and security)

#### 1.1. HTTP/1.1 Requests Only

#### 1.2. Remove C++ Exceptions

#### 1.3. Memory Management

#### 1.4. GET Parameter Parsing

#### 1.5. Limits Checking

#### 1.6. Result Queuing

#### 1.7. Tests / Code Quality

#### 1.8. C++11 Conformity

### 2. HTTP Response Generator

#### 2.1. Result Code

#### 2.2. Result Message

#### 2.3. Date Header

#### 2.4. Adding Custom Headers

## Version 0.3

- Remove Result Processor / ThreadHandler for results processing completely
- Move Results Processing into main server process / thread
- This removes necesserity of clientFD passing between processes / threads
- This also removes / reduces shared memory processing to application server processes only
- Fix object relation between ClientHandler -> Client -> ProtocolParser
- Implement XML protocol parser *library* (with similar result queuing functionality to HTTPParser) - c++ move semantics
- Remove ResultOrering, implement RequestUUID handling
- Integrate parsing / encryption (threaded) with fixed thread count and atomic locked *request* **and** *result* queue(s)

## Future

- According to changes in `v0.3` implement NLAFP sendfile() single-threaded / in a single process
- NETCONF integration
