# HTTP Library (internal)

The HTTP Library is split up in three parts:

- httpnet.cpp / Network Processing (used in Tests) 
- httpparser.cpp / HTTP Parsing (Header, Payload) 
- httpgen.cpp / HTTP Generating (Header)

## Compiling / Linking

The HTTPLib library gets linked shared because it wil be used by multiple applications (e.g. test).

## Program Logic (httpparser.cpp)

The parser is programmed to support the following:

- Multiple HTTP "messages" in one TCP packet
- Fragmented (partial) messages without end marker "\n\r"

### appendBuffer()

1. Params

- const char* BufferRef
- const uint16_t BufferSize

2. Processing Logic

The passed buffer data will be apended to _HTTPRequest private member. Afterwards _splitRequests()
method will be called (see next topic).

### _splitRequests()

- _SplittedRequests Vector will be cleared
- All requests inside _HTTPRequest buffer get split by "\n\r" HTTP end marker
- Single requests will be put inside _SplittedRequests Vector

### parseRequestsBasic()

1. Params

void* SHMStaticFS
void* SHMPostAS

2. Processing Logic

- Store StaticFS Shared Mem Address Pointer inside object instance
- Store AS POST Shared Mem Address Pointer inside object instance

- Loop (for each _SplittedRequests element) call this._processBasePropsSHM()

### _processBasePropsSHM

1. Params

string& Request

2. Processing Logic

- Get HTTP Version
- Get HTTP Payload
- Get HTTP Payload size (bytes)

- Check Request Type (StaticFS / Python Script POST AS)

- On Type SatticFS (SHM Segment #1)

  - Write ClientFD to SHM-StaticFS, increment Pointer Address
  - Write HTTPVersion to SHM-StaticFS, increment Pointer Address
  - Write MsgNr to SHM-StaticFS, increment Pointer Address
  - Write MsgLength to SHM-StaticFS, increment Pointer Address
  - Write MsgPayload to SHM-StaticFS, increment Pointer Address

  > IPCHandler.cpp is used to calculate Shared Memory Adress Offsets.

- On Type POST AS (SHM Segment #2, #3)

  // SHM Segment #3
  - Get Next Free AS Index (Check CanRead == 0 && WriteReady == 0)
    - If no Free AS:: add to RequestQueue

  // SHM Segment #2
  - Write ClientFD to SHM-PostAS @AS Index Address
  - Write HTTPVersion to SHM-PostAS @AS Index Address
  - Write MsgNr to SHM-PostAS @AS Index Address
  - Write MsgLength to SHM-PostAS @AS Index Address
  - Write MsgPayload to SHM-PostAS @AS Index Address

  // SHM Segment #3
  - Write CanRead = 1 @AS Index Address

  > IPCHandlerAS.cpp is used to calculate Shared Memory Adress Offsets.