# 1. HTTP Library (internal)

The HTTP Library is split up in three parts:

- httpnet.cpp / Network Processing (Tests)
- httpparser.cpp / HTTP Parsing (Header, Payload)
- httpgen.cpp / HTTP Generating (Header)

## 1.1. Compiling / Linking

The HTTPLib library gets linked shared because it wil be used by multiple applications (e.g. test).

## 1.2. Program Logic (httpparser.cpp)

Features:

- GET Requests (without Content-Length header)
- POST Requests (with Content-Length header and bytes[size] after "\n\r" end marker
- Multiple HTTP "messages" in one TCP packet
- Fragmented (partial) messages without end marker "\n\r" (GET) or "Content-Length" (POST)

### 1.2.1. appendBuffer()

#### 1.2.1.1. Params

1. const char* BufferRef
2. const uint16_t BufferSize

#### 1.2.1.2. Processing Logic

The passed buffer data will be apended to _HTTPRequest private member. Afterwards _splitRequests()
method will be called.

### 1.2.2. _splitRequests()

1. _SplittedRequests Vector will be cleared
2. All requests inside _HTTPRequest buffer get split by "\n\r" or "\n\r"+Content-Length
3. Single requests will be put inside _SplittedRequests Vector

### 1.2.3. parseRequestsBasic()

#### 1.2.3.1. Params

1. SharedMemAddress_t SHMGetRequests
2. const ASRequestHandlerRef_t ASRequestHandlerRef

#### 1.2.3.2. Processing Logic

1. Set SHM Base Address (SHMGetRequests)
2. Foreach _SplittedRequests Vector Element: call _parseBaseProps(&Request, ASRequestHandlerRef)


### 1.2.4. _parseBaseProps

#### 1.2.4.1. Params

1. string& Request
2. const ASRequestHandlerRef_t ASRequestHandlerRef

#### 1.2.4.2. Processing Logic

- Get HTTP Version (HTTP/1.1 or HTTP/1.2)
- Get HTTP Method (GET/POST)
- Get HTTP Payload
- Get HTTP Payload Size (Bytes)

- Check Request Type (StaticFS / Python Script POST AS)

- On Type SatticFS (SHM Segment #1)

  - Write ClientFD to SHM-StaticFS, increment Pointer Address
  - Write HTTPVersion to SHM-StaticFS, increment Pointer Address
  - Write MsgNr to SHM-StaticFS, increment Pointer Address
  - Write MsgLength to SHM-StaticFS, increment Pointer Address
  - Write MsgPayload to SHM-StaticFS, increment Pointer Address

> IPCHandler.hpp and IPCHandler.cpp is used to calculate Shared Memory Adress Offsets.

- On Type POST AS (SHM Segment #2/Metadata, #3/RequestPayloads)

  // SHM Segment #2
  - Get Next Free AS Index (Check CanRead == 0 && WriteReady == 0)
    - If no Free AS:: add to RequestQueue

  // SHM Segment #2
  - Write ClientFD to SHM-PostAS @AS Index Address
  - Write HTTPVersion to SHM-PostAS @AS Index Address
  - Write HTTPMethod to SHM-PostAS @AS Index Address
  - Write ReqNr to SHM-PostAS @AS Index Address
  - Write ReqPayloadLength to SHM-PostAS @AS Index Address

  //- SHM Segment #3
  - Write ReqPayload to SHM-PostAS @AS Index Address

  // SHM Segment #2
  - Write CanRead = 1 @AS Index Address

> IPCHandlerAS.hpp and IPCHandlerAS.cpp is used to calculate Shared Memory Adress Offsets.
