# 1. Application Server Process Handler

The Application Server Process Handler manages:

- Setup Application Server Processes / Instances (fork)
- SHM Requests / Synchronization for all AS Instances
- SHM Writes / Synchronization for all AS Instances

## 1.1. Program Logic

### 1.1.1. Initialization

```c++
ASIndex = 0
- Loop on Config::VirtualDomains
  - Loop on VirtualDomain::ASCount
    - Fork AS[Index] Process / SHM Reference
    - Index++
```

#### 1.1.1.1. ASProcessHandler Initialization Workflow

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

### 1.1.2. Main Loop

```c++
- Loop
  - Check CanRead == 1 && WriteReady == 0
    - If yes: Process Python Request
    - Write CanRead == 0 && WriteReady = 1
```

#### 1.1.2.1. ASProcessHandler Main Loop Workflow

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
