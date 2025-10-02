# 1. Client Handler

The ClientHandler Component handles Client Sockets / Epoll Setup / Data Buffering.

## 1.1. Program Logic

### 1.1.1. Add Client

Called from Main::Server on new Client Socket.

- Insert ClientObject with ClientFD as Key into internal C++ Map
- Add ClientFD to Kernel Epoll

#### 1.1.1.1. Add Client Workflow

```mermaid
flowchart TD
    A[addClient ClientFD] --> B[Set Socket Non-blocking]
    B --> C[Create HTTPParser Object]
    C --> D[Insert Client to Map]
    D --> E[Setup Epoll Event EPOLLIN|EPOLLET]
    E --> F[Add FD to Epoll]
```

### 1.1.2. Process Clients

Get all Epoll FDs with waiting data.

> If 5000 current Clients are connected and 2000 have waiting buffered data, we get an
Array of these 2000 Filedescriptor integers.

Process all Filedescriptors calling readClientData().

#### 1.1.2.1. Process Clients Workflow

```mermaid
flowchart TD
    A[processClients] --> B[Reset ProcessedClients = 0]
    B --> C[epoll_wait for Ready FDs]
    C --> D{Error?}
    D -->|Yes| E[Log Error and Return]
    D -->|No| F{FDCount > 0?}
    F -->|Yes| G[readClientData FDCount]
    F -->|No| H[Process AppServer Queue]
    G --> H
    H --> I[Update ProcessedClients]
```

### 1.1.3. Read Client Data

Process all Filedescriptors with waiting data.

- Close connection when connection-close (0 Bytes received) on Socket Level
- If Filedesriptor in Client Map found, append received data to Client Objects Buffer
- Parse Request Basic Data, if non-fragmented add to SHM buffer
- If Clients with data exist, release SHM StaticFS Lock to process

#### 1.1.3.1. Read Client Data Workflow

```mermaid
flowchart TD
    A[readClientData FDCount] --> B[Initialize SHM Pointers]
    B --> C[Loop Through Ready FDs]
    C --> D{More FDs?}
    D -->|Yes| E[Get ClientFD from Epoll Events]
    E --> F[Receive Data from Socket]
    F --> G{Bytes Received?}
    G -->|0 bytes| H[Close Connection]
    H --> I[Remove from Epoll]
    I --> J[Remove from Client Map]
    J --> D
    G -->|> 0 bytes| K{Client in Map?}
    K -->|No| D
    K -->|Yes| L[Append Data to Buffer]
    L --> M[Parse HTTP Request]
    M --> N{Request Complete?}
    N -->|Yes| O{GET Request?}
    O -->|Yes| P[Add to StaticFS SHM]
    O -->|No| Q{POST Request?}
    Q -->|Yes| R[Add to AS Request Queue]
    R --> D
    P --> S[Increment Request Count]
    S --> D
    N -->|No| D
    D -->|No| T{Requests Added?}
    T -->|Yes| U[Release StaticFS Lock]
    T -->|No| V[Return]
    U --> V
```
