# HTTP/1.2 Protocol Specification - Draft

**Version:** 0.1 DRAFT  
**Date:** October 2024  
**Status:** Request for Proposal (RFP)  
**Authors:** Claus Prüfer / clickIT / WEBcodeX  
**Contact:** http1-2@webcodex.de  

---

## Abstract

This document specifies HTTP/1.2, a simplified and optimized evolution of HTTP/1.1 that fixes the broken pipelining mechanism while maintaining the elegant simplicity of text-based protocols. HTTP/1.2 eliminates the need for complex Layer-7 multiplexing by introducing a Request-UUID header system, allowing responses to be matched to requests regardless of network ordering. This specification defines the core protocol, message formats, headers, and operational requirements for HTTP/1.2 implementations.

---

## Table of Contents

1. [Introduction](#1-introduction)
2. [Protocol Overview](#2-protocol-overview)
3. [Terminology](#3-terminology)
4. [Protocol Architecture](#4-protocol-architecture)
5. [Connection Management](#5-connection-management)
6. [Message Format](#6-message-format)
7. [Header Fields](#7-header-fields)
8. [Request Methods](#8-request-methods)
9. [Status Codes](#9-status-codes)
10. [Pipelining and Request-UUID](#10-pipelining-and-request-uuid)
11. [Caching](#11-caching)
12. [Security Considerations](#12-security-considerations)
13. [Implementation Guidelines](#13-implementation-guidelines)
14. [Feature Comparison](#14-feature-comparison)
15. [References](#15-references)
16. [Appendices](#16-appendices)

---

## 1. Introduction

### 1.1. Purpose

HTTP/1.2 is designed to address the fundamental flaw in HTTP/1.1's pipelining implementation while avoiding the excessive complexity introduced by HTTP/2. The protocol maintains backward compatibility with HTTP/1.1 where possible, while introducing minimal changes to enable reliable request pipelining.

### 1.2. Motivation

**The Problem with HTTP/1.1:**
- Pipelining is broken due to response ordering requirements
- Responses must arrive in the same order as requests
- Network latency and processing variations make this impractical
- Led to widespread disabling of pipelining

**The Problem with HTTP/2:**
- Excessive complexity destroys HTTP/1.1's elegant simplicity
- Layer-7 multiplexing duplicates existing TCP/IP Layer-3 features
- Binary protocol makes debugging and analysis difficult
- Unnecessary coupling of TLS/SSL with the protocol
- Confusion in developer community about implementation

**The Problem with HTTP/3:**
- UDP-based approach works only for precalculated CDN data
- Adds complexity without solving core architectural issues
- Flow control reimplemented at application layer
- Not suitable for general-purpose web applications

**The HTTP/1.2 Solution:**
- Single new header field: `Request-UUID`
- Fixes pipelining with minimal protocol changes
- Leverages existing TCP/IP multiplexing capabilities
- Maintains text-based simplicity
- No unnecessary complexity

### 1.3. Design Principles

1. **Simplicity First**: Minimal changes to HTTP/1.1
2. **Leverage Existing Infrastructure**: Use TCP/IP Layer-3 features
3. **Text-Based Protocol**: Maintain debuggability and clarity
4. **Performance**: Enable true request pipelining
5. **Security**: Process isolation and clean architecture
6. **Compatibility**: Easy migration path from HTTP/1.1

### 1.4. Scope

This specification covers:
- Core HTTP/1.2 protocol semantics
- Request-UUID mechanism for pipelined requests
- Connection management and permanent keep-alive
- Message syntax and header fields
- Caching for static content
- Security considerations

This specification does NOT cover:
- TLS/SSL implementation (handled separately)
- WebSocket protocol (separate specification)
- Byte-range requests (use specialized streaming protocols)
- Cross-Origin Resource Sharing (CORS) (deprecated in favor of clean application architecture)

---

## 2. Protocol Overview

### 2.1. Protocol Identification

HTTP/1.2 is identified by the protocol version string `HTTP/1.2` in the request and response lines.

**Request Line Format:**
```
GET /path/to/resource HTTP/1.2
```

**Response Line Format:**
```
HTTP/1.2 200 OK
```

### 2.2. Key Features

#### 2.2.1. Permanent Keep-Alive Connections

HTTP/1.2 uses **permanent** keep-alive by default. One client establishes exactly **one TCP socket** to a server domain/virtual host and maintains this connection for all requests.

**Rationale:** TCP/IP multiplexing at Layer-3 is well-tested and reliable. Multiple data channels can coexist on a single socket without blocking or retransmission problems.

#### 2.2.2. Request-UUID System

Every HTTP/1.2 request includes a `Request-UUID` header containing a unique identifier. The server echoes this UUID in the response, allowing the client to match responses to requests regardless of arrival order.

**Example:**
```
GET /api/data HTTP/1.2
Host: example.com
Request-UUID: 70936869-3b17-4bcd-89fb-72adf57acc27
```

Response:
```
HTTP/1.2 200 OK
Request-UUID: 70936869-3b17-4bcd-89fb-72adf57acc27
Content-Type: application/json
Content-Length: 42

{"status": "success", "data": "value"}
```

#### 2.2.3. Simplified Feature Set

HTTP/1.2 **omits** the following features:
- **Chunked Transfer Encoding**: Unnecessary for modern high-speed networks
- **Compressed Transfer Encoding**: Use pre-compressed assets, not runtime compression
- **Byte-Range Requests**: Use specialized streaming protocols instead
- **Parametrized Keep-Alive**: Connection is always permanent
- **HTTP Methods**: Only GET and POST (see Section 8)

---

## 3. Terminology

### 3.1. Key Terms

- **Client**: The user agent initiating HTTP/1.2 requests
- **Server**: The program accepting HTTP/1.2 connections and serving responses
- **Request**: A complete HTTP/1.2 message sent from client to server
- **Response**: A complete HTTP/1.2 message sent from server to client
- **Request-UUID**: A universally unique identifier for a request/response pair
- **Permanent Keep-Alive**: A TCP connection maintained for the lifetime of the client session
- **Pipelining**: Sending multiple requests without waiting for responses
- **Virtual Host**: A logical server identified by the Host header

### 3.2. Notation

This specification uses the following notation:
- `MUST`, `MUST NOT`: Absolute requirements
- `SHOULD`, `SHOULD NOT`: Recommendations
- `MAY`, `OPTIONAL`: Truly optional features

---

## 4. Protocol Architecture

### 4.1. Client-Server Model

HTTP/1.2 follows the traditional client-server model with the following characteristics:

```
+----------------+     TCP/IP Socket      +----------------+
|                |  <----------------->   |                |
|  HTTP/1.2      |   Permanent Keep-Alive |  HTTP/1.2      |
|  Client        |   Request Pipelining   |  Server        |
|                |   Request-UUID Based   |                |
+----------------+                        +----------------+
```

### 4.2. Connection Lifecycle

1. **Connection Establishment**: Client opens TCP connection to server
2. **Permanent Keep-Alive**: Connection remains open for session lifetime
3. **Request Pipelining**: Client sends multiple requests without waiting
4. **Response Matching**: Client uses Request-UUID to match responses
5. **Connection Termination**: Explicit close by client or server

### 4.3. Process Architecture (Reference Implementation)

The FalconAS reference implementation uses a multi-process architecture:

```
+----------------+---------------+---------------+----------------+
| Server Process | AS Process 1  | AS Process x  | Result Process |
|                | Python Interp.| Python Interp.|                |
+-----------------------------------------------------------------+
| Shared Memory                                                   |
|  - StaticFS Requests                                            |
|  - AS Metadata                                                  |
|  - AS Requests                                                  |
|  - AS Results                                                   |
+-----------------------------------------------------------------+
```

**Key Components:**
- **Server Process**: Epoll-based connection handling
- **AS Processes**: Application server with embedded interpreters (Python/Java)
- **Result Process**: Response processing and ordering
- **Shared Memory**: IPC with atomic user-space locks (kernel mutex-less)

---

## 5. Connection Management

### 5.1. Permanent Keep-Alive

#### 5.1.1. Establishment

HTTP/1.2 connections are **always** persistent. Upon successful TCP connection establishment:

1. Client MAY send multiple requests immediately (pipelining)
2. Server MUST maintain connection state for the session
3. Connection remains open until explicit termination

#### 5.1.2. Connection Reuse

Unlike HTTP/1.1 with optional keep-alive:
- No `Connection: keep-alive` header needed
- No timeout negotiation required
- Single socket per client-server pair
- Eliminates connection overhead

#### 5.1.3. Connection Termination

Connections are terminated only when:
1. Client explicitly closes the socket
2. Server must restart or shutdown
3. Network error occurs
4. Server sends `Connection: close` header (exceptional cases only)

#### 5.1.4. Rationale

**No Parametrized Keep-Alive:**
- Security: Clients should not modify server parameters
- Simplicity: Eliminates negotiation logic
- Performance: Permanent connections are optimal

---

## 6. Message Format

### 6.1. Request Format

HTTP/1.2 requests follow this structure:

```
Request-Line
Header-Field-1
Header-Field-2
...
Header-Field-N
<CRLF>
[Message-Body]
```

#### 6.1.1. Request Line

```
Method SP Request-URI SP HTTP-Version CRLF
```

**Example:**
```
GET /index.html HTTP/1.2
POST /api/submit HTTP/1.2
```

#### 6.1.2. Request Headers

Minimum required headers for all requests:
- `Host`: Target virtual host
- `Request-UUID`: Unique request identifier

**Example Request:**
```
GET /api/users HTTP/1.2
Host: api.example.com
Request-UUID: a3f2b8c1-9d7e-4a5b-8c3f-1e2d3c4b5a6f
User-Agent: FalconClient/1.0
Accept: application/json

```

### 6.2. Response Format

HTTP/1.2 responses follow this structure:

```
Status-Line
Header-Field-1
Header-Field-2
...
Header-Field-N
<CRLF>
[Message-Body]
```

#### 6.2.1. Status Line

```
HTTP-Version SP Status-Code SP Reason-Phrase CRLF
```

**Example:**
```
HTTP/1.2 200 OK
HTTP/1.2 404 Not Found
```

#### 6.2.2. Response Headers

Minimum required headers for all responses:
- `Request-UUID`: Echo of request UUID
- `Content-Type`: Media type of response body
- `Content-Length`: Size of response body in bytes

**Example Response:**
```
HTTP/1.2 200 OK
Request-UUID: a3f2b8c1-9d7e-4a5b-8c3f-1e2d3c4b5a6f
Content-Type: application/json
Content-Length: 87
Date: Mon, 07 Oct 2024 12:00:00 GMT

{"users": [{"id": 1, "name": "Alice"}, {"id": 2, "name": "Bob"}]}
```

### 6.3. Message Body

- Body is OPTIONAL for GET requests
- Body is REQUIRED for POST requests with payload
- Body length MUST match `Content-Length` header
- No chunked encoding (entire body sent as single unit)

---

## 7. Header Fields

### 7.1. Standard Headers

#### 7.1.1. Request-UUID (REQUIRED)

**Syntax:**
```
Request-UUID: <uuid-v4>
```

**Description:**  
Universally unique identifier for request/response correlation. MUST be a valid UUID v4 format.

**Example:**
```
Request-UUID: 70936869-3b17-4bcd-89fb-72adf57acc27
```

**Client Behavior:**
- Client MUST generate unique UUID for each request
- Client MUST use UUID v4 format (RFC 4122)
- Client MUST store UUID for response matching

**Server Behavior:**
- Server MUST echo Request-UUID in response
- Server MUST preserve UUID exactly as received

#### 7.1.2. Host (REQUIRED)

**Syntax:**
```
Host: <hostname>[:<port>]
```

**Description:**  
Specifies the virtual host and port for the request.

**Example:**
```
Host: www.example.com
Host: api.example.com:8080
```

#### 7.1.3. Content-Type

**Syntax:**
```
Content-Type: <media-type>
```

**Supported Media Types:**
- `text/html`
- `application/json`
- `application/xml`
- `text/plain`
- `application/octet-stream`

**Example:**
```
Content-Type: application/json; charset=utf-8
```

#### 7.1.4. Content-Length

**Syntax:**
```
Content-Length: <length-in-bytes>
```

**Description:**  
Size of message body in bytes. REQUIRED when body is present.

**Example:**
```
Content-Length: 1024
```

#### 7.1.5. User-Agent

**Syntax:**
```
User-Agent: <product-name>/<version>
```

**Example:**
```
User-Agent: FalconClient/1.0
User-Agent: Mozilla/5.0 (compatible; HTTP1.2Client/1.0)
```

#### 7.1.6. Accept

**Syntax:**
```
Accept: <media-type>[, <media-type>]*
```

**Example:**
```
Accept: application/json, text/html
```

### 7.2. Caching Headers

#### 7.2.1. If-None-Match

**Syntax:**
```
If-None-Match: "<etag>"
```

**Description:**  
Used for conditional requests. Server returns 304 if ETag matches.

**Example:**
```
If-None-Match: "686897696a7c876b7e"
```

#### 7.2.2. ETag

**Syntax:**
```
ETag: "<entity-tag>"
```

**Description:**  
Entity tag for cache validation (static content only).

**Example:**
```
ETag: "686897696a7c876b7e"
```

#### 7.2.3. Cache-Control

**Syntax:**
```
Cache-Control: <directive>
```

**Supported Directives:**
- `public`: Response may be cached
- `private`: Response is for single user
- `no-cache`: Must revalidate
- `max-age=<seconds>`: Maximum cache lifetime

**Example:**
```
Cache-Control: public, max-age=3600
```

### 7.3. Connection Headers

#### 7.3.1. Connection

**Syntax:**
```
Connection: close
```

**Description:**  
OPTIONAL. Used only when server must close connection. Default behavior is permanent keep-alive.

---

## 8. Request Methods

### 8.1. Supported Methods

HTTP/1.2 supports only two request methods:

#### 8.1.1. GET

**Purpose:** Retrieve resource

**Characteristics:**
- MUST NOT have side effects
- SHOULD NOT include request body
- Responses MAY be cached

**Example:**
```
GET /api/users/123 HTTP/1.2
Host: api.example.com
Request-UUID: 7ea45c8a-5193-4855-b9e8-77ae1b9d49ed
Accept: application/json

```

#### 8.1.2. POST

**Purpose:** Submit data for processing

**Characteristics:**
- MAY have side effects
- MUST include request body (when submitting data)
- Responses SHOULD NOT be cached

**Example:**
```
POST /api/users HTTP/1.2
Host: api.example.com
Request-UUID: 3f8d2a1b-4c5e-6f7a-8b9c-0d1e2f3a4b5c
Content-Type: application/json
Content-Length: 45

{"name": "Alice", "email": "alice@example.com"}
```

### 8.2. Unsupported Methods

The following HTTP/1.1 methods are **NOT** supported in HTTP/1.2:

- **HEAD**: Use GET and ignore body
- **PUT**: Use POST to web service endpoints
- **DELETE**: Use POST to web service endpoints
- **OPTIONS**: CORS deprecated, not needed
- **TRACE**: Security risk, not needed
- **CONNECT**: Proxy functionality separate
- **PATCH**: Use POST to web service endpoints

**Rationale:**
- Modern web applications use REST APIs via GET/POST
- Document-based methods are obsolete
- Cleaner security model
- Reduced complexity

---

## 9. Status Codes

### 9.1. Success Codes (2xx)

#### 9.1.1. 200 OK

**Description:** Request succeeded

**Use Cases:**
- Successful GET request
- Successful POST request with data

**Example:**
```
HTTP/1.2 200 OK
Request-UUID: 7ea45c8a-5193-4855-b9e8-77ae1b9d49ed
Content-Type: application/json
Content-Length: 27

{"status": "success"}
```

### 9.2. Redirection Codes (3xx)

#### 9.2.1. 304 Not Modified

**Description:** Resource not modified (cache validation)

**Use Cases:**
- Client sent If-None-Match
- ETag matches current version

**Example:**
```
HTTP/1.2 304 Not Modified
Request-UUID: 7ea45c8a-5193-4855-b9e8-77ae1b9d49ed
ETag: "686897696a7c876b7e"

```

### 9.3. Client Error Codes (4xx)

#### 9.3.1. 400 Bad Request

**Description:** Malformed request

**Example:**
```
HTTP/1.2 400 Bad Request
Request-UUID: 7ea45c8a-5193-4855-b9e8-77ae1b9d49ed
Content-Type: text/plain
Content-Length: 23

Invalid request format
```

#### 9.3.2. 404 Not Found

**Description:** Resource does not exist

**Example:**
```
HTTP/1.2 404 Not Found
Request-UUID: 7ea45c8a-5193-4855-b9e8-77ae1b9d49ed
Content-Type: text/html
Content-Length: 142

<html><body><h1>404 Not Found</h1><p>The requested resource was not found.</p></body></html>
```

### 9.4. Server Error Codes (5xx)

#### 9.4.1. 500 Internal Server Error

**Description:** Server encountered error processing request

**Example:**
```
HTTP/1.2 500 Internal Server Error
Request-UUID: 7ea45c8a-5193-4855-b9e8-77ae1b9d49ed
Content-Type: application/json
Content-Length: 45

{"error": "Internal server error occurred"}
```

---

## 10. Pipelining and Request-UUID

### 10.1. The Pipelining Problem in HTTP/1.1

HTTP/1.1 pipelining failed because:

1. **Strict Ordering Requirement**: Responses MUST arrive in request order
2. **Head-of-Line Blocking**: Slow request blocks all subsequent responses
3. **Network Variability**: Processing time and network latency vary
4. **Implementation Complexity**: Difficult to implement correctly

**Result:** Pipelining disabled by default in most browsers

### 10.2. The HTTP/1.2 Solution

HTTP/1.2 solves pipelining with Request-UUID:

1. **Client Behavior:**
   - Generate unique UUID for each request
   - Include UUID in `Request-UUID` header
   - Send requests immediately (no waiting)
   - Store UUID-to-callback mapping

2. **Server Behavior:**
   - Process requests as they arrive
   - Echo UUID in response
   - Send responses as soon as ready (any order)

3. **Response Matching:**
   - Client receives response
   - Extracts `Request-UUID` from headers
   - Looks up original request by UUID
   - Invokes appropriate callback/handler

### 10.3. Example: Pipelined Requests

**Client sends three requests immediately:**

```
GET /api/users HTTP/1.2
Host: api.example.com
Request-UUID: uuid-001

GET /api/posts HTTP/1.2
Host: api.example.com
Request-UUID: uuid-002

GET /api/comments HTTP/1.2
Host: api.example.com
Request-UUID: uuid-003

```

**Server responds as each completes (any order):**

```
HTTP/1.2 200 OK
Request-UUID: uuid-003
Content-Length: 50

[{"id": 1, "text": "Great post!"}]
```

```
HTTP/1.2 200 OK
Request-UUID: uuid-001
Content-Length: 45

[{"id": 1, "name": "Alice"}]
```

```
HTTP/1.2 200 OK
Request-UUID: uuid-002
Content-Length: 60

[{"id": 1, "title": "HTTP/1.2 Overview"}]
```

**Client matches by UUID, not order.**

### 10.4. Benefits

1. **True Multiplexing**: Leverages TCP/IP Layer-3 features
2. **No Head-of-Line Blocking**: Fast requests don't wait for slow ones
3. **Simple Implementation**: No complex stream management
4. **Debuggable**: Text-based protocol, visible UUIDs
5. **Backward Compatible**: Similar to HTTP/1.1 with one new header

---

## 11. Caching

### 11.1. Caching Scope

HTTP/1.2 supports caching **only for static content**:

- HTML files
- CSS stylesheets
- JavaScript files
- Images
- Other static assets

### 11.2. Cache Validation

#### 11.2.1. ETag-Based Validation

**First Request:**
```
GET /styles.css HTTP/1.2
Host: www.example.com
Request-UUID: uuid-001

```

**First Response:**
```
HTTP/1.2 200 OK
Request-UUID: uuid-001
Content-Type: text/css
Content-Length: 2048
ETag: "abc123def456"
Cache-Control: public, max-age=3600

[CSS content]
```

**Subsequent Request:**
```
GET /styles.css HTTP/1.2
Host: www.example.com
Request-UUID: uuid-002
If-None-Match: "abc123def456"

```

**304 Response (Not Modified):**
```
HTTP/1.2 304 Not Modified
Request-UUID: uuid-002
ETag: "abc123def456"

```

### 11.3. Cache Directives

**For Static Content:**
- `Cache-Control: public, max-age=<seconds>`
- `ETag: "<tag>"`

**For Dynamic Content:**
- `Cache-Control: no-cache`
- No ETag header

---

## 12. Security Considerations

### 12.1. Process Isolation

The reference implementation uses process-based isolation:

- **Server Process**: Connection handling (minimal privileges)
- **Application Processes**: Isolated interpreters (sandboxed)
- **Result Process**: Response assembly (isolated)

**Benefits:**
- Python GIL not an issue (separate processes)
- Crash isolation (one process failure doesn't affect others)
- Security boundaries (reduced attack surface)

### 12.2. No CORS

HTTP/1.2 **does not support** Cross-Origin Resource Sharing (CORS):

**Rationale:**
- Clean application architecture eliminates need
- Security by design: same-origin only
- Complexity reduction
- Modern container/Kubernetes environments handle routing

### 12.3. TLS/SSL

HTTP/1.2 is **transport-independent**:

- TLS/SSL is **NOT** part of the protocol specification
- HTTPS uses HTTP/1.2 over TLS (standard practice)
- TLS termination can occur at:
  - Client-server connection
  - Load balancer/reverse proxy
  - API gateway

### 12.4. Request Size Limits

Implementations SHOULD enforce:
- Maximum header size (e.g., 8 KB)
- Maximum body size (configurable)
- Maximum URL length (e.g., 2048 bytes)

### 12.5. Denial of Service Protection

Implementations SHOULD implement:
- Connection rate limiting
- Request rate limiting per connection
- Maximum concurrent connections
- Timeout enforcement

---

## 13. Implementation Guidelines

### 13.1. Client Implementation

#### 13.1.1. UUID Generation

Clients MUST generate RFC 4122 UUID v4:

**Example (Python):**
```python
import uuid

request_uuid = str(uuid.uuid4())
headers = {
    'Host': 'api.example.com',
    'Request-UUID': request_uuid
}
```

#### 13.1.2. Response Matching

Maintain mapping of UUID to request context:

**Example (JavaScript):**
```javascript
const pendingRequests = new Map();

function sendRequest(url, callback) {
  const uuid = generateUUID();
  pendingRequests.set(uuid, callback);
  
  const request = `GET ${url} HTTP/1.2\r\n` +
                  `Host: api.example.com\r\n` +
                  `Request-UUID: ${uuid}\r\n\r\n`;
  
  socket.write(request);
}

function handleResponse(response) {
  const uuid = extractUUID(response);
  const callback = pendingRequests.get(uuid);
  pendingRequests.delete(uuid);
  callback(response);
}
```

### 13.2. Server Implementation

#### 13.2.1. Request Parsing

Parse request line and headers:

**Example (C++):**
```cpp
void HTTPParser::parseRequest(const string& request) {
    // Split request into lines
    vector<string> lines = split(request, "\r\n");
    
    // Parse request line
    auto [method, uri, version] = parseRequestLine(lines[0]);
    
    // Parse headers
    map<string, string> headers;
    for (size_t i = 1; i < lines.size(); i++) {
        if (lines[i].empty()) break;
        auto [name, value] = parseHeader(lines[i]);
        headers[name] = value;
    }
    
    // Extract UUID
    string uuid = headers["Request-UUID"];
    
    // Process request...
}
```

#### 13.2.2. Response Generation

Always echo Request-UUID:

**Example (Python):**
```python
def generate_response(request_uuid, content):
    response = f"HTTP/1.2 200 OK\r\n"
    response += f"Request-UUID: {request_uuid}\r\n"
    response += f"Content-Type: application/json\r\n"
    response += f"Content-Length: {len(content)}\r\n"
    response += "\r\n"
    response += content
    return response
```

### 13.3. Performance Optimization

#### 13.3.1. Epoll-Based I/O

Use epoll (Linux) for scalable connection handling:

```cpp
int epoll_fd = epoll_create1(0);

struct epoll_event event;
event.events = EPOLLIN | EPOLLET;
event.data.fd = socket_fd;

epoll_ctl(epoll_fd, EPOLL_CTL_ADD, socket_fd, &event);

while (running) {
    struct epoll_event events[MAX_EVENTS];
    int n = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
    
    for (int i = 0; i < n; i++) {
        handleConnection(events[i].data.fd);
    }
}
```

#### 13.3.2. Shared Memory IPC

For multi-process architectures, use shared memory with atomic locks:

```cpp
// Allocate shared memory
void* shm = mmap(NULL, size, PROT_READ | PROT_WRITE,
                 MAP_SHARED | MAP_ANONYMOUS, -1, 0);

// Atomic lock (user-space, kernel mutex-less)
std::atomic<uint16_t>* lock = 
    new(shm) std::atomic<uint16_t>(0);

// Acquire lock
uint16_t expected = 0;
while (!lock->compare_exchange_weak(expected, 1)) {
    expected = 0;
}

// Critical section...

// Release lock
lock->store(0);
```

#### 13.3.3. Huge Pages

Enable huge pages for better memory performance:

```bash
# Enable transparent huge pages
echo always > /sys/kernel/mm/transparent_hugepage/enabled

# Allocate with huge pages
void* mem = mmap(NULL, size, PROT_READ | PROT_WRITE,
                 MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB,
                 -1, 0);
```

---

## 14. Feature Comparison

### 14.1. HTTP/1.1 vs HTTP/1.2 vs HTTP/2

| Feature                           | HTTP/1.1              | HTTP/1.2                    | HTTP/2                   |
|-----------------------------------|-----------------------|-----------------------------|--------------------------|
| Keep-Alive                        | Optional              | Permanent (always)          | Always                   |
| Pipelining                        | Broken (ordering)     | **Working** (Request-UUID)  | N/A (multiplexed)        |
| Multiplexing                      | No                    | TCP/IP Layer-3              | Layer-7 (complex)        |
| Protocol Type                     | Text                  | Text                        | Binary                   |
| Header Compression                | No                    | No                          | HPACK                    |
| Server Push                       | No                    | No                          | Yes                      |
| Request Methods                   | GET, POST, PUT, etc.  | GET, POST only              | All methods              |
| Chunked Encoding                  | Yes                   | No                          | Yes (frames)             |
| Byte Range Requests               | Yes                   | No                          | Yes                      |
| CORS Support                      | Yes                   | No                          | Yes                      |
| Complexity                        | Low                   | **Very Low**                | High                     |
| Debuggability                     | Easy (text)           | **Easy** (text)             | Hard (binary)            |
| Performance                       | Moderate              | **High**                    | High                     |

### 14.2. Key Advantages of HTTP/1.2

1. **Simplicity**: Minimal changes from HTTP/1.1
2. **Working Pipelining**: Request-UUID fixes fundamental flaw
3. **Debuggability**: Text-based protocol
4. **Performance**: Leverages TCP/IP multiplexing
5. **Security**: Process isolation architecture
6. **No Bloat**: Removes unnecessary features

---

## 15. References

### 15.1. Normative References

- **RFC 2616**: Hypertext Transfer Protocol -- HTTP/1.1
- **RFC 4122**: A Universally Unique IDentifier (UUID) URN Namespace
- **RFC 7230**: Hypertext Transfer Protocol (HTTP/1.1): Message Syntax and Routing
- **RFC 7231**: Hypertext Transfer Protocol (HTTP/1.1): Semantics and Content

### 15.2. Informative References

- **RFC 7540**: Hypertext Transfer Protocol Version 2 (HTTP/2)
- **RFC 9114**: HTTP/3
- **HTTP/1.2 Project**: https://github.com/WEBcodeX1/http-1.2
- **Der IT Prüfer**: http://der-it-pruefer.de - Detailed HTTP protocol analysis
- **FalconAS Documentation**: http://docs.webcodex.de/wacp/rfp/

### 15.3. Implementation References

- **FalconAS**: HTTP/1.2 reference implementation (C++, Python, Java)
- **Feature Matrix**: See FEATURE-MATRIX.md in project repository
- **WAP/XML Specifications**: See WAP-AS-XML-SPECS.md

---

## 16. Appendices

### 16.A. Complete Request/Response Examples

#### 16.A.1. Simple GET Request

**Request:**
```
GET /api/users/123 HTTP/1.2
Host: api.example.com
Request-UUID: 7ea45c8a-5193-4855-b9e8-77ae1b9d49ed
User-Agent: FalconClient/1.0
Accept: application/json

```

**Response:**
```
HTTP/1.2 200 OK
Request-UUID: 7ea45c8a-5193-4855-b9e8-77ae1b9d49ed
Content-Type: application/json
Content-Length: 87
Date: Mon, 07 Oct 2024 12:00:00 GMT

{"id": 123, "name": "Alice", "email": "alice@example.com", "created": "2024-01-01"}
```

#### 16.A.2. POST Request with JSON

**Request:**
```
POST /api/users HTTP/1.2
Host: api.example.com
Request-UUID: 3f8d2a1b-4c5e-6f7a-8b9c-0d1e2f3a4b5c
User-Agent: FalconClient/1.0
Content-Type: application/json
Content-Length: 67

{"name": "Bob", "email": "bob@example.com", "role": "developer"}
```

**Response:**
```
HTTP/1.2 200 OK
Request-UUID: 3f8d2a1b-4c5e-6f7a-8b9c-0d1e2f3a4b5c
Content-Type: application/json
Content-Length: 89

{"id": 124, "name": "Bob", "email": "bob@example.com", "role": "developer", "created": "2024-10-07"}
```

#### 16.A.3. Pipelined Requests

**Client Sends (all at once):**
```
GET /api/users HTTP/1.2
Host: api.example.com
Request-UUID: uuid-001

GET /api/posts HTTP/1.2
Host: api.example.com
Request-UUID: uuid-002

GET /api/comments HTTP/1.2
Host: api.example.com
Request-UUID: uuid-003

```

**Server Responds (in completion order):**
```
HTTP/1.2 200 OK
Request-UUID: uuid-003
Content-Type: application/json
Content-Length: 82

[{"id": 1, "text": "Great!", "user_id": 1}, {"id": 2, "text": "Thanks", "user_id": 2}]
```

```
HTTP/1.2 200 OK
Request-UUID: uuid-001
Content-Type: application/json
Content-Length: 65

[{"id": 1, "name": "Alice"}, {"id": 2, "name": "Bob"}]
```

```
HTTP/1.2 200 OK
Request-UUID: uuid-002
Content-Type: application/json
Content-Length: 78

[{"id": 1, "title": "First Post", "user_id": 1, "created": "2024-10-01"}]
```

#### 16.A.4. Static File with Caching

**First Request:**
```
GET /styles/main.css HTTP/1.2
Host: www.example.com
Request-UUID: cache-001

```

**First Response:**
```
HTTP/1.2 200 OK
Request-UUID: cache-001
Content-Type: text/css
Content-Length: 2048
ETag: "abc123def456"
Cache-Control: public, max-age=86400

body { font-family: Arial; }
/* ... more CSS ... */
```

**Subsequent Request (with cache validation):**
```
GET /styles/main.css HTTP/1.2
Host: www.example.com
Request-UUID: cache-002
If-None-Match: "abc123def456"

```

**304 Response (cache still valid):**
```
HTTP/1.2 304 Not Modified
Request-UUID: cache-002
ETag: "abc123def456"

```

### 16.B. UUID Generation Examples

#### 16.B.1. Python
```python
import uuid

def generate_request_uuid():
    return str(uuid.uuid4())

# Example usage
request_id = generate_request_uuid()
# Output: "7ea45c8a-5193-4855-b9e8-77ae1b9d49ed"
```

#### 16.B.2. JavaScript
```javascript
function generateUUID() {
  return 'xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx'.replace(/[xy]/g, function(c) {
    const r = Math.random() * 16 | 0;
    const v = c === 'x' ? r : (r & 0x3 | 0x8);
    return v.toString(16);
  });
}

// Example usage
const requestId = generateUUID();
// Output: "3f8d2a1b-4c5e-6f7a-8b9c-0d1e2f3a4b5c"
```

#### 16.B.3. C++
```cpp
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

std::string generateUUID() {
    boost::uuids::uuid uuid = boost::uuids::random_generator()();
    return boost::uuids::to_string(uuid);
}

// Example usage
std::string requestId = generateUUID();
// Output: "70936869-3b17-4bcd-89fb-72adf57acc27"
```

### 16.C. Migration from HTTP/1.1

#### 16.C.1. Client Migration

**HTTP/1.1 Code:**
```python
import requests

response = requests.get('http://api.example.com/users')
data = response.json()
```

**HTTP/1.2 Code:**
```python
import requests
import uuid

headers = {
    'Request-UUID': str(uuid.uuid4())
}

response = requests.get(
    'http://api.example.com/users',
    headers=headers
)

data = response.json()
```

#### 16.C.2. Server Migration

**Key Changes:**
1. Always echo `Request-UUID` header in responses
2. Enable permanent keep-alive (disable timeout-based closure)
3. Process requests in any order
4. Remove chunked encoding support
5. Limit methods to GET and POST

### 16.D. Performance Considerations

#### 16.D.1. Why Not Coroutines?

While coroutines are trending, they can be counterproductive:

**Problem:**
- One `await read()` per connection = one syscall
- 10,000 connections = 10,000 syscalls
- Massive overhead for checking received data

**Epoll Advantage:**
- Single `epoll_wait()` informs about ALL ready file descriptors
- One syscall handles thousands of connections
- Used by nginx and other high-performance servers

#### 16.D.2. Shared Memory vs Message Passing

**HTTP/1.2 Approach (Shared Memory):**
- Atomic user-space locks (no kernel mutex)
- Zero-copy data sharing
- Minimal context switching

**Alternative (Message Passing):**
- Kernel involvement for IPC
- Data copying overhead
- More context switches

### 16.E. Why No Byte Range Requests?

**Rationale:**
- HTTP/1.2 targets modern web applications, not streaming
- Streaming should use specialized protocols (HLS, DASH, RTMP)
- Reduces protocol complexity
- Intel XEON 6 + 800Gbit Ethernet = sub-second page loads

**For Streaming:**
- Use existing protocols designed for streaming
- HTTP/1.2 focuses on web application performance

### 16.F. Why No Compressed Transfer Encoding?

**Rationale:**
- Runtime compression wastes CPU cycles
- Modern networks (800Gbit Ethernet) make it unnecessary
- Use pre-compressed assets instead
- Environmental consideration: unnecessary power consumption

**Better Approach:**
- Web-Pack format: single compressed metadata+media package
- Send once at initial request
- Update only when app changes
- Reduce socket count by orders of magnitude

### 16.G. Process Architecture Benefits

**FalconAS Implementation:**

```
Server Process (epoll)
├── Static File Handler (sendfile)
└── Request Parser (HTTP/1.2)

AS Process 1 (Python Interpreter)
├── Shared Memory Reader
└── Business Logic

AS Process N (Python Interpreter)
├── Shared Memory Reader
└── Business Logic

Result Process
├── Response Assembler
└── Result Scheduler (UUID-based)
```

**Benefits:**
1. **No GIL Issues**: Each process has own Python interpreter
2. **Crash Isolation**: One process failure doesn't affect others
3. **Security**: Process boundaries provide isolation
4. **Scalability**: Processes distributed across CPU cores
5. **Debugging**: Easier to trace and debug separate processes

---

## 17. Acknowledgments

HTTP/1.2 builds upon the excellent work of:

- The HTTP/1.1 specification authors
- The web development community
- Open-source contributors to the FalconAS project
- Reviewers and early adopters

Special thanks to Claus Prüfer for the vision and implementation.

---

## 18. Author's Address

**Claus Prüfer**  
clickIT / WEBcodeX  
Email: http1-2@webcodex.de  
Web: http://docs.webcodex.de/wacp/rfp/  

**Project Repository:**  
https://github.com/WEBcodeX1/http-1.2  

**Issue Tracker:**  
https://github.com/WEBcodeX1/http-1.2/issues  

---

## 19. Copyright Notice

Copyright (c) 2024 Claus Prüfer / clickIT / WEBcodeX

This document is subject to the GNU Affero General Public License v3.0.

---

**Document Status:** DRAFT  
**Version:** 0.1  
**Last Updated:** October 2024  
**Next Review:** Upon community feedback
