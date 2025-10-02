# 1. Result Processor

The Result Processor manages the following tasks:

- Check / Read StaticFS Requests
- Check / Read AS POST Results
- Result Send Thread Processor

## 1.1. Read StaticFS Requests

If Lock::get_read_data (Server Main Loop) == 1, SHM Data with StaticFS requests is ready to read.

Loop on all GET requests and feed into `ResultProcessor::ResultOrder::appendData(ClientFD, DataPointer)`.

### 1.1.1. Read StaticFS Requests Workflow

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

## 1.2. Read AS Results

Check all AS Instances if WriteReady == 1.

If WriteReady == 1 (Existing result):

- `ResultProcessor::ResultOrder::appendData(ClientFD, DataPointer)`
- Reset AS Instance Properties to default (CanRead == 1, WriteReady = 0)

### 1.2.1. Read AS Results Workflow

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

## 1.3. Result Scheduler

Result for HTTP/1.1 must be synchronous / ordered.

Result Scheduler Class definition see: [ResultScheduler.md](./ResultScheduler.md)

## 1.4. Main Loop

Main Loop Layout.

```bash
loop:

  workdone = 0

  - if StaticFSLock == 1
      append GET static FS requests into ResultProcessor::ResultOrder
      StaticFSLock = 0
      workdone = 1
  - get_post_results()
      if AS::Index1::WriteReady == 1
        append POST result into ResultProcessor::ResultOrder::appendData(ClientFD, DataPointer)
        set CanRead = 1, WriteReady = 0
        workdone = 1
      if AS::IndexX::WriteReady == 1
        append POST result into ResultProcessor::ResultOrder::appendData(ClientFD, DataPointer)
        workdone = 1
        set CanRead = 1, WriteReady = 0
  - if fds_to_process
      ThreadProcessor::processThreads
      workdone = 1

  - if workdone == 0:
    sleep Config::Microseconds
```

### 1.4.1. ResultProcessor Main Loop Workflow

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

### 1.4.1. On Idle

If workdone == 0 (no work done), micro-sleep.

## 1.5. Result Send Thread Processor

```bash
+-------------+-----------+------------------------------+
| ClientFD    | RequestNr | Request                      |
+-------------+-----------+------------------------------+
| 212         | 1         | GET /bla/one.jpg             |
+-------------+-----------+------------------------------+
| 212         | 2         | GET /bla/two.jpg             |
+-------------+-----------+------------------------------+
| 212         | 3         | GET /bla/one.html            |
+-------------+-----------+------------------------------+
| 403         | 1         | GET /bla/one.jpg             |
+-------------+-----------+------------------------------+
| 403         | 2         | GET /bla/two.jpg             |
+-------------+-----------+------------------------------+
| 403         | 3         | GET /bla/one.html            |
+-------------+-----------+------------------------------+
| 403         | 4         | POST /test.py                |
+-------------+-----------+------------------------------+
```

### 1.5.0. ThreadHandler Process Workflow

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

### 1.5.1. Thread 1 (detached)

Thread 1 will sendfile() three results to ClientFD 212 socket.

```bash
+-------------+-----------+------------------------------+
| ClientFD    | RequestNr | Request                      |
+-------------+-----------+------------------------------+
| 212         | 1         | GET /bla/one.jpg             |
+-------------+-----------+------------------------------+
| 212         | 2         | GET /bla/two.jpg             |
+-------------+-----------+------------------------------+
| 212         | 3         | GET /bla/one.html            |
+-------------+-----------+------------------------------+
```

Thread sets terminate flag to 1 after execution and waits to be terminated.

#### 1.5.1.1. ClientThread Processing Workflow

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

### 1.5.2. Thread 2 (detached)

If AS result has been processed in time, Thread2 will sendfile() the first 3 results and
afterwards write() the AS result to ClientFD 403 socket.

```bash
+-------------+-----------+------------------------------+
| ClientFD    | RequestNr | Request                      |
+-------------+-----------+------------------------------+
| 403         | 1         | GET /bla/one.jpg             |
+-------------+-----------+------------------------------+
| 403         | 2         | GET /bla/two.jpg             |
+-------------+-----------+------------------------------+
| 403         | 3         | GET /bla/one.html            |
+-------------+-----------+------------------------------+
| 403         | 4         | POST /test.py                |
+-------------+-----------+------------------------------+
```

Thread sets terminate flag to 1 after execution and waits to be terminated.
