# Signal Termination Handling Test

## Overview

This integration test verifies that the server properly handles SIGTERM signals and propagates them to all child processes.

## Test Description

The test validates the following behavior:

1. **Parent Process Tracking**: When child processes are forked, their PIDs are registered with the parent process
2. **Signal Propagation**: When the parent process receives a SIGTERM signal, it sends SIGTERM to all tracked child processes
3. **Clean Shutdown**: All child processes handle SIGTERM gracefully and exit cleanly

## Test Cases

### Test 1: Parent Sends SIGTERM to Children
- Creates a parent process with two child processes
- Registers child PIDs with the parent
- Sends SIGTERM to parent
- Verifies that parent forwards SIGTERM to all children
- Verifies that all processes exit cleanly with status 0

### Test 2: Child Processes Not Killed Without SIGTERM
- Creates a parent with a child process
- Verifies that without receiving SIGTERM, the child continues running
- Sends SIGTERM to verify clean shutdown capability

## How It Works

The termination handling in the HTTP server follows this pattern:

1. **Server.cpp**: Main server process
   - Maintains a static vector `ChildPIDs` to track all child process IDs
   - `Server::terminate()` handler sends SIGTERM to all tracked children when parent receives SIGTERM

2. **ASProcessHandler.cpp**: Application Server child processes
   - Each forked AS process PID is registered via `registerChildPID()`
   - Child processes have their own SIGTERM handlers to exit gracefully

3. **ResultProcessor.cpp**: Result processor child process
   - Forked process PID is returned and registered with parent
   - Has its own SIGTERM handler for clean shutdown

## Running the Test

```bash
cd build/test/integration/signal-termination
./test-signal-termination
```

## Expected Output

All test cases should pass, indicating:
- ✓ Parent successfully tracks child PIDs
- ✓ SIGTERM is propagated from parent to all children
- ✓ All processes exit cleanly without requiring SIGKILL
