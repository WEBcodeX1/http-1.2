# 1. Main Server

The Main Server Process initializes all components on startup using configuation from
XML Configuration file.

## 1.1. Program Logic

### 1.1.1. Initialization

- Setup Shared Memory Segments
- Disable Signals / Setup Termination Handling
- Setup Server Listening Socket / Poll for Server Socket Filedescriptor
- Load / Init StaticFS recursive Filesystem Snapshot for each Virtual Host
- Init AS Handler Process (Fork all Application Server Processes)
- Init Result Processor (Fork Result Processor Process)
- Setup Connection Handler Instance
- Drop System Privileges
- Enter Server Loop

#### 1.1.1.1. Server Initialization Workflow

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

### 1.1.2. Main Loop

```c++
- While Static::RunServer is True
  - Check for Socket Accept Client (New Connection)
    - Add Client FD to Connection Handler
  - Process Clients with waiting data (Connection Handler Method)
```

#### 1.1.2.1. ServerLoop Workflow

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
