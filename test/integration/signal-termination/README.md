# 1. Signal Termination Integration Test

This directory contains a self-contained Boost.Test executable that models the server's
child-process shutdown flow.

## 1.1. Contents

- `test-signal-termination.cpp`: forks helper child processes, installs `SIGTERM`
  handlers, and verifies orderly shutdown behavior.
- `CMakeLists.txt`: registers the `test-signal-termination` executable.

## 1.2. Covered Behavior

The test mirrors the shutdown pattern implemented in the runtime sources without starting
the full server:

- `src/Server.cpp`: keeps track of child PIDs and terminates them during shutdown.
- `src/ASProcessHandler.cpp`: registers forked application-server processes with the
  server-side PID tracker.

The Boost.Test cases verify that:

1. child PIDs can be collected by the parent-side tracker,
2. sending `SIGTERM` to the tracked children terminates them cleanly, and
3. a child process continues running until an explicit termination signal is sent.

## 1.3. Running the Test

```bash
cd build/test/integration/signal-termination
./test-signal-termination
```
