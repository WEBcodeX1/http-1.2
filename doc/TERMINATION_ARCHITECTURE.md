# Termination Handling - Visual Architecture

## Process Hierarchy

```
                    ┌─────────────────────────────────┐
                    │   Main Server Process (root)    │
                    │  - Handles SIGTERM               │
                    │  - Tracks child PIDs             │
                    │  - Propagates signals            │
                    └────────────┬────────────────────┘
                                 │
                    ┌────────────┴────────────┐
                    │                         │
        ┌───────────▼──────────┐  ┌──────────▼──────────────┐
        │  ResultProcessor     │  │  ASProcessHandler (x N)  │
        │  - Child PID: 1234   │  │  - Child PIDs: 1235...N  │
        │  - Handles SIGTERM   │  │  - Handle SIGTERM        │
        │  - Process results   │  │  - Run app servers       │
        └──────────────────────┘  └─────────────────────────┘
```

## Signal Flow

```
Step 1: User issues termination command
        $ pkill falcon-as
                │
                ▼
        ┌────────────────┐
        │  SIGTERM sent  │
        └───────┬────────┘
                │
                ▼
        ┌───────────────────────────────────┐
        │  Main Server Signal Handler       │
        │  Server::terminate(int signal)    │
        └───────┬───────────────────────────┘
                │
                ▼
        ┌───────────────────────────────────┐
        │  Server::terminateChildren()      │
        │  - Iterate over ChildPIDs vector  │
        │  - kill(pid, SIGTERM) for each    │
        └───────┬───────────────────────────┘
                │
        ┌───────┴────────┐
        │                │
        ▼                ▼
   ┌─────────┐    ┌─────────────────┐
   │ Child 1 │    │ Child 2...N     │
   │ SIGTERM │    │ SIGTERM         │
   └────┬────┘    └────┬────────────┘
        │              │
        ▼              ▼
   Exit cleanly   Exit cleanly
```

## PID Registration Flow

```
Main Server Process
    │
    ├─→ Fork ResultProcessor
    │      │
    │      ├─→ Parent: receive child PID (1234)
    │      │     └─→ Server::addChildPID(1234)
    │      │           └─→ ChildPIDs.push_back(1234)
    │      │
    │      └─→ Child: execute ResultProcessor
    │
    └─→ Fork ASProcessHandler (for each namespace/interpreter)
           │
           ├─→ Parent: receive child PID (1235)
           │     └─→ ASProcessHandler::registerChildPID(1235)
           │           └─→ registerChildPIDToServer(1235)
           │                 └─→ Server::addChildPID(1235)
           │                       └─→ ChildPIDs.push_back(1235)
           │
           └─→ Child: execute ASProcessHandler
```

## Before vs After

### Before Implementation
```
$ ps aux | grep falcon
root     1000  falcon-as (main)
www-data 1001  falcon-as-0
www-data 1002  falcon-as-1
www-data 1003  falcon-as-2

$ kill -TERM 1000
(Only main process exits, others orphaned)

$ ps aux | grep falcon
www-data 1001  falcon-as-0  ← Still running!
www-data 1002  falcon-as-1  ← Still running!
www-data 1003  falcon-as-2  ← Still running!

$ kill -9 1001 1002 1003  ← Manual cleanup required
```

### After Implementation
```
$ ps aux | grep falcon
root     2000  falcon-as (main)
www-data 2001  falcon-as-0
www-data 2002  falcon-as-1
www-data 2003  falcon-as-2

$ kill -TERM 2000
(Main propagates SIGTERM to all children)

$ ps aux | grep falcon
(All processes exit cleanly - nothing remains)
```

## Code Modifications Summary

### Minimal Changes Required

1. **Server.hpp** (+3 declarations)
   - `static std::vector<pid_t> ChildPIDs`
   - `static void addChildPID(pid_t)`
   - `static void terminateChildren()`

2. **Server.cpp** (+20 lines)
   - Initialize ChildPIDs
   - Implement addChildPID()
   - Implement terminateChildren()
   - Modify terminate() to call terminateChildren()
   - Register ResultProcessor PID

3. **ASProcessHandler.cpp** (+6 lines)
   - Implement registerChildPID()
   - Call registerChildPID() after each fork

4. **ResultProcessor.cpp** (+2 lines)
   - Change return type to pid_t
   - Return child PID

Total source changes: ~31 lines of code changes to core functionality
