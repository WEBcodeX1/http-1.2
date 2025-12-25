14. Termination Handling
========================

14.1. Problem Statement
-----------------------

Previously, the HTTP server only responded to SIGKILL (``kill -9``), not to SIGTERM, because:

1. The main server PID (root) would receive SIGTERM and set ``RunServer`` to ``false``
2. However, child PIDs (ResultProcessor, multiple ASProcessHandler PIDs) were not notified
3. Since child processes run as non-root, they are not automatically terminated on parent exit
4. This required manual cleanup with ``kill -9`` for each process

14.2. Solution
--------------

Implemented proper termination handling where the main server process tracks all child PIDs and propagates SIGTERM signals to them.

14.3. How It Works
------------------

14.3.1. Process Hierarchy
~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: text

   Main Server (root, then drops privileges)
   ├── ResultProcessor (child process 1)
   └── ASProcessHandler (multiple child processes, one per namespace interpreter)

14.3.2. Termination Flow
~~~~~~~~~~~~~~~~~~~~~~~~

1. User sends ``kill -TERM <main_pid>`` or ``pkill falcon-as``
2. Main server process receives SIGTERM
3. Signal handler ``Server::terminate()`` is invoked
4. ``Server::terminateChildren()`` sends SIGTERM to all registered child PIDs
5. Each child process receives SIGTERM and their handlers set ``RunServer = false``
6. All processes exit their main loops cleanly
7. Parent waits for children to exit (implicit via fork behavior)

14.3.3. Key Design Decisions
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

1. **Static Members**: ``ChildPIDs`` is static to be accessible from the signal handler context
2. **Global Function**: ``registerChildPIDToServer()`` breaks Server, ASProcessHandler dependency
3. **PID Registration**: Done in parent process immediately after fork succeeds
4. **Signal Order**: Children receive SIGTERM before parent exits

14.4. Testing
-------------

A comprehensive test suite validates:

- Parent tracks child PIDs correctly
- SIGTERM propagates from parent to all children
- All processes exit cleanly without SIGKILL
- Processes don't terminate prematurely without SIGTERM

14.5. Benefits
--------------

1. **Clean Shutdown**: Server responds to standard SIGTERM signal
2. **No Orphan Processes**: All children are properly terminated
3. **Resource Cleanup**: Allows processes to clean up resources before exit
4. **Standard Behavior**: Follows Unix/Linux process management conventions
5. **Systemd Compatible**: Works correctly with systemd service management

14.6. Usage
-----------

.. code-block:: bash

   # start server
   ./usr/local/bin/falcon-as

   # stop server (now works correctly)
   pkill falcon-as
   # or
   kill -TERM <pid>
