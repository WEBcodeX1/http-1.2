# Preface

Our intention is to build good, stable and secure software. This implies detailed documentation.

> Do not forget: keep the clean code paradigm in mind.

The following document describes the Base-Layout of `FalconAS` / `HTTP/1.2`, its Base Components
and Insfrastructural Layout.

# Logical Components

The following components are referenced throughout the documentation.

> Logical Component Names used in documentation and the C++ Abstraction Model sometimes do not match.

## Configuration

XML Configuration Handling. C++ Transformation.

## Main::Server

Main Server Loop.

[Main-Server.md](./Main-Server.md)

## Main::ClientHandler

Client / Connection Handler.

[ClientHandler.md](./ClientHandler.md)

## Main::StaticFSHandler

Static Files / Virtual Host Handler.

[StaticFSHandler.md](./StaticFSHandler.md)

## ASProcessHandler

Python Application Server Process Handler.

[ASProcessHandler.md](./ASProcessHandler.md)

## ResultProcessor

Result processing process (fork).

[ResultProcessor.md](./ResultProcessor.md)

## ResultProcessor::ThreadHandler

Result Thread Distributor.

Due to looping on Result sendfile(), write() and Header Processing, each ClientFDs request**s** (plural)
will be processed by a single Posix Thread.

This also guarantees that multiple repeated result writes (in case of congestion) always will be
processed in serial order.

Big results using multiple writes are sent in the correct order as well.

## ResultProcessor::ResultOrder

Result Ordering will be done for the following types:

- Static Files, sendfile()
- Application Server Results (JSON), write()

### HTTP/1.1

Result Ordering. Results must be sent in the exact order how they arrived.
The Result Ordering Class keeps track of dependend ClientFD Requests / Request Nr. and related.

### HTTP/1.2

No order needed. Our implementation appends the "Request-UUID" HTTP Header to the HTTP Response.

# Shared Memory Layout

Due to the Python Global Interpreter Lock the `Main Server`, `ResultProcessor` and each Application
Server Process are implemented using Unix Processes.

Data Sharing (Requests, Synchronization) is done by Shared Memory and User Space Atomic Locks
(Kernel Mutex less).

```bash
+-----------------------+-----------------------+-----------------------+-----------------------+
| Server Process        | AS Process 1          | AS Process x          | Result Processor      |
|                       | Python Interpreter    | Python Interpreter    |                       |
+-----------------------+-----------------------+-----------------------+-----------------------+
| Shared Mem                                                                                    |
|  - StaticFS Requests                                                                          |
|  - AS Requests                                                                                |
|  - AS Results                                                                                 |
+-----------------------------------------------------------------------------------------------+
```

> 32bit memory addresses used for simplicity.

## HTTP StaticFS Requests #1

```bash
Address                 Type                Descr           Default

0x00000000              atomic_uint64_t     StaticFSLock    0
0x00000008              uint16_t            RequestCount    0

-- Req 1 Metadata ---------------------------------------------------

0x0000000a              uint16_t            ClientFD        Nullptr
0x0000000c              uint8_t             HTTPVersion     Nullptr
0x0000000d              uint16_t            RequestNr       Nullptr
0x0000000f              uint16_t            PayloadLength   Nullptr
0x00000011              char[]              char[LenReq1]   Nullptr

-- Req 2 Metadata ---------------------------------------------------

0x00000011+LenReq1      uint16_t            ClientFD        Nullptr
0x00000011+LenReq1+2    uint8_t             HTTPVersion     Nullptr
0x00000011+LenReq1+3    uint16_t            RequestNr       Nullptr
0x00000011+LenReq1+5    uint16_t            PayloadLength   Nullptr
0x00000011+LenReq1+7    char[]              char[LenReq1]   Nullptr
```

## HTTP AS Requests #2

```bash
Address                 Type                Descr           Default

-- AS 1 Metadata ----------------------------------------------------

0x00000000              uint16_t            PayloadLength   Nullptr
0x00000002              char[]              char[LenReq1]   Nullptr

-- AS 2 Metadata ----------------------------------------------------

0x00000002+LenReq1      uint16_t            PayloadLength   Nullptr
0x00000002+LenReq1+2    char[]              char[LenReq1]   Nullptr
```

## Application Server Status / Results #3

```bash
Address                 Type                Descr           Default

-- AS 1 Metadata ---------------------------------------------------

0x00000000              atomic_uint8_t      CanRead         0
0x00000001              atomic_uint8_t      WriteReady      0

0x00000002              uint64_t            ClientFD        Nullptr
0x0000000a              uint8_t             HTTPVersion     1
0x0000000b              uint16_t            ReqNr           1
0x0000000d              uint32_t            PayloadLength   Nullptr

-- AS 2 Metadata ---------------------------------------------------

0x00000011              atomic_uint8_t      CanRead         0
0x00000012              atomic_uint8_t      WriteReady      0

0x00000013              uint64_t            ClientFD        Nullptr
0x0000001b              uint8_t             HTTPVersion     1
0x0000001c              uint16_t            ReqNr           1
0x0000001e              uint32_t            PayloadLength   Nullptr

-- AS 1 Payload ----------------------------------------------------

0x00000022              char[PayloadLenA1]  Payload         Nullptr

-- AS 2 Payload ----------------------------------------------------

0x00000022+max_pl_bytes char[PayloadLenA2]  Payload         Nullptr
```
