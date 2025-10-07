# 1. Graphical Workflows

This document contains all graphical workflow diagrams extracted from the specification documents.

## 1.1. Application Server Process Handler Workflows

### 1.1.1. ASProcessHandler Initialization Workflow

```mermaid
flowchart TD
    A[forkProcessASHandler] --> B[ASIndex = 0]
    B --> C{More VirtualDomains?}
    C -->|Yes| D{More Interpreters for Domain?}
    D -->|Yes| E[Fork AS Process]
    E --> F{Parent Process?}
    F -->|Yes| G[Register Child PID]
    G --> H[Increment ASIndex]
    H --> D
    F -->|No| I[Child: Setup PID FD to Parent]
    I --> J[Set Termination Handler]
    J --> K[Setup SHM Pointers]
    K --> L[Initialize Backend Processor]
    L --> M[Enter Main Loop]
    D -->|No| C
    C -->|No| N[All AS Processes Forked]
```

### 1.1.2. ASProcessHandler Main Loop Workflow

```mermaid
flowchart TD
    A[AS Main Loop] --> B{RunServer == True?}
    B -->|Yes| C[Get SHM Meta Addresses]
    C --> D{CanRead == 1 AND WriteReady == 0?}
    D -->|Yes| E[Read Request from SHM]
    E --> F[Process Request via Backend]
    F --> G[Write Result to SHM]
    G --> H[Set CanRead = 0, WriteReady = 1]
    H --> B
    D -->|No| I[Check Parent Process Alive]
    I --> J{Parent Alive?}
    J -->|No| K[Exit Loop]
    J -->|Yes| L[Sleep Microseconds]
    L --> B
    B -->|No| M[AS Process Exit]
    K --> M
```

## 1.2. Client Handler Workflows

### 1.2.1. Add Client Workflow

```mermaid
flowchart TD
    A[addClient ClientFD] --> B[Set Socket Non-blocking]
    B --> C[Create HTTPParser Object]
    C --> D[Insert Client to Map]
    D --> E[Setup Epoll Event EPOLLIN,EPOLLET]
    E --> F[Add FD to Epoll]
```

### 1.2.2. Process Clients Workflow

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

### 1.2.3. Read Client Data Workflow

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

## 1.3. Main Server Workflows

### 1.3.1. Server Initialization Workflow

```mermaid
flowchart TD
    A[Server::init] --> B[Setup Shared Memory]
    B --> C[Set Shared Memory Pointers]
    C --> D[Init Static Filesystem]
    D --> E[Set Client Handler Config]
    E --> F[Configure Socket Address/Port]
    F --> G[Disable OS Signals SIGINT, SIGPIPE]
    G --> H[Setup Termination Handler]
    H --> I[Setup Server Socket]
    I --> J[Setup Poll for Server Socket]
    J --> K[Get ASRequestHandler Reference]
    K --> L[Fork Result Processor Process]
    L --> M[Register ResultProcessor PID]
    M --> N[Fork Application Server Processes]
    N --> O[Check AS Interpreter Count]
    O --> P[Drop System Privileges]
    P --> Q[Enter ServerLoop]
```

### 1.3.2. ServerLoop Workflow

```mermaid
flowchart TD
    A[ServerLoop Start] --> B{RunServer == True?}
    B -->|Yes| C[Poll Server FD for Connections]
    C --> D{Poll Error?}
    D -->|Yes| E[Log Error]
    E --> B
    D -->|No| F{POLLIN Event?}
    F -->|Yes| G[acceptClient]
    G --> H[Add Client to ClientHandler]
    H --> I[processClients]
    I --> B
    F -->|No| J{ProcessedClients == 0?}
    J -->|Yes| K[Sleep IDLE_SLEEP_MICROSECONDS]
    K --> I
    J -->|No| I
    B -->|No| L[Server Exit]
```

## 1.4. Result Processor Workflows

### 1.4.1. Read StaticFS Requests Workflow

```mermaid
flowchart TD
    A[Check StaticFS Lock] --> B{StaticFSLock == 1?}
    B -->|Yes| C[Read Request Count from SHM]
    C --> D[Loop Through Requests]
    D --> E{More Requests?}
    E -->|Yes| F[Read Request Metadata]
    F --> G[Parse HTTP Properties]
    G --> H[Append to ResultOrder]
    H --> E
    E -->|No| I[Set StaticFSLock = 0]
    I --> J[Mark Work Done]
    B -->|No| K[Continue]
```

### 1.4.2. Read AS Results Workflow

```mermaid
flowchart TD
    A[Check AS Instances] --> B[Loop Through AS Instances]
    B --> C{More Instances?}
    C -->|Yes| D[Get Meta Addresses]
    D --> E{WriteReady == 1?}
    E -->|Yes| F[Read Result Metadata]
    F --> G[Read Result Payload]
    G --> H[Append to ResultOrder]
    H --> I[Set CanRead = 1, WriteReady = 0]
    I --> J[Mark Work Done]
    J --> C
    E -->|No| C
    C -->|No| K[Return Result Count]
```

### 1.4.3. ResultProcessor Main Loop Workflow

```mermaid
flowchart TD
    A[ResultProcessor Main Loop] --> B{RunServer == True?}
    B -->|Yes| C[WorkDone = False]
    C --> D[Reset ResultOrder]
    D --> E{StaticFS Lock == 1?}
    E -->|Yes| F[Process StaticFS Requests]
    F --> G[WorkDone = True]
    G --> H[Process AS Results]
    E -->|No| H
    H --> I{Results Processed?}
    I -->|Yes| J[WorkDone = True]
    I -->|No| K[Calculate ResultOrder]
    J --> K
    K --> L[Process HTTP/1.2 Requests]
    L --> M[Process HTTP/1.1 Requests]
    M --> N{Requests to Process?}
    N -->|Yes| O[ThreadHandler::processThreads]
    O --> P[WorkDone = True]
    P --> Q{WorkDone == False?}
    N -->|No| Q
    Q -->|Yes| R[Sleep Microseconds]
    R --> S{Parent Process Alive?}
    Q -->|No| S
    S -->|Yes| B
    S -->|No| T[Exit Loop]
    B -->|No| U[ResultProcessor Exit]
    T --> U
```

### 1.4.4. ThreadHandler Process Workflow

```mermaid
flowchart TD
    A[ThreadHandler::processThreads] --> B[Sort Requests by ClientFD]
    B --> C[Add to ProcessRequests Queue]
    C --> D[Loop Through ProcessRequests]
    D --> E{More Requests?}
    E -->|Yes| F{Thread Exists for ClientFD?}
    F -->|Yes| G[Skip - Thread in Progress]
    G --> E
    F -->|No| H[Create ClientThread Object]
    H --> I[Store Thread Index]
    I --> J[Start Thread]
    J --> E
    E -->|No| K[Check Processed Threads]
    K --> L{Thread Joinable?}
    L -->|Yes| M[Join Thread]
    M --> N[Remove from ProcessRequests]
    N --> O[Remove from Index Map]
    O --> P[Remove from ClientThreads]
    P --> L
    L -->|No| Q[Return]
```

### 1.4.5. ClientThread Processing Workflow

```mermaid
flowchart TD
    A[ClientThread::processRequests] --> B[Loop Through Client Requests]
    B --> C{More Requests?}
    C -->|Yes| D{Request Type?}
    D -->|StaticFS| E[Lookup File in VHost Map]
    E --> F{File Found?}
    F -->|Yes| G[Build HTTP Response Header]
    G --> H[Send Header via write]
    H --> I[Send File via sendfile]
    I --> C
    F -->|No| J[Build 404 Response]
    J --> K[Send 404 via write]
    K --> C
    D -->|AppServer| L[Build HTTP Response Header]
    L --> M[Send Header via write]
    M --> N[Send AS Result via write]
    N --> C
    C -->|No| O[Close Client Socket]
    O --> P[Set Thread Complete Flag]
```
