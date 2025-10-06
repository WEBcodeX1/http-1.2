Termination Handling
====================

Problem Statement
-----------------

Previously, the HTTP server only responded to SIGKILL (``kill -9``), not to SIGTERM. This is because:

1. The main server process (running as root) would receive SIGTERM and set its ``RunServer`` flag to ``false``
2. However, child processes (ResultProcessor and multiple ASProcessHandler processes) were not notified
3. Since child processes run as non-root users, they are not automatically terminated when the parent exits
4. This required manual cleanup with ``kill -9`` for each process

Solution
--------

Implemented proper termination handling where the main server process tracks all child PIDs and propagates SIGTERM signals to them.

How It Works
------------

Process Hierarchy
~~~~~~~~~~~~~~~~~

.. code-block:: text

   Main Server (root, then drops privileges)
   ├── ResultProcessor (child process 1)
   └── ASProcessHandler (multiple child processes, one per namespace interpreter)

Termination Flow
~~~~~~~~~~~~~~~~

1. User sends ``kill -TERM <main_pid>`` or ``pkill falcon-as``
2. Main server process receives SIGTERM
3. Signal handler ``Server::terminate()`` is invoked
4. ``Server::terminateChildren()`` sends SIGTERM to all registered child PIDs
5. Each child process receives SIGTERM and their handlers set ``RunServer = false``
6. All processes exit their main loops cleanly
7. Parent waits for children to exit (implicit via fork behavior)

Key Design Decisions
~~~~~~~~~~~~~~~~~~~~

1. **Static Members**: ``ChildPIDs`` is static to be accessible from the signal handler context
2. **Global Function**: ``registerChildPIDToServer()`` breaks circular dependency between Server and ASProcessHandler
3. **PID Registration**: Done in parent process immediately after fork succeeds
4. **Signal Order**: Children receive SIGTERM before parent exits

Testing
-------

A comprehensive test suite validates:

- Parent tracks child PIDs correctly
- SIGTERM propagates from parent to all children
- All processes exit cleanly without SIGKILL
- Processes don't terminate prematurely without SIGTERM

Benefits
--------

1. **Clean Shutdown**: Server responds to standard SIGTERM signal
2. **No Orphan Processes**: All children are properly terminated
3. **Resource Cleanup**: Allows processes to clean up resources before exit
4. **Standard Behavior**: Follows Unix/Linux process management conventions
5. **Systemd Compatible**: Works correctly with systemd service management

Usage
-----

.. code-block:: bash

   # Start server
   ./usr/local/bin/falcon-as

   # Stop server (now works correctly)
   pkill falcon-as
   # or
   kill -TERM <pid>

   # No longer need kill -9
