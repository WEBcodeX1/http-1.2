14. Termination Handling
========================

14.1. Problem Statement
-----------------------

The current shutdown path is designed around a main server process that may register backend child
processes and propagate ``SIGTERM`` to them.

1. The main server PID receives ``SIGTERM`` and flips ``RunServer`` to ``false``
2. Any child PIDs registered through ``Server::addChildPID()`` are stored in ``Server::ChildPIDs``
3. ``Server::terminateChildren()`` sends ``SIGTERM`` to those PIDs in reverse registration order
4. The shutdown flow is safe even when no child processes were spawned

14.2. Solution
--------------

Implemented proper termination handling where the main server process tracks all child PIDs and propagates SIGTERM signals to them.

14.3. How It Works
------------------

14.3.1. Process Hierarchy
~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: text

   Main Server (root, then drops privileges)
   └── ASProcessHandler children (optional, when backend workers are enabled)

14.3.2. Termination Flow
~~~~~~~~~~~~~~~~~~~~~~~~

1. User sends ``kill -TERM <main_pid>`` or ``pkill falcon-as``
2. Main server process receives SIGTERM
3. Signal handler ``Server::terminate()`` is invoked
4. ``Server::terminateChildren()`` sends ``SIGTERM`` to all registered child PIDs
5. Each child process receives ``SIGTERM`` and its handler sets ``RunServer = false``
6. All active loops exit cleanly
7. If no children were registered, the main server simply exits its own loop

14.3.3. Key Design Decisions
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

1. **Static Members**: ``ChildPIDs`` is static to be accessible from the signal handler context
2. **Global Function**: ``registerChildPIDToServer()`` breaks the direct ``Server`` /
   ``ASProcessHandler`` dependency
3. **PID Registration**: Done in parent process immediately after fork succeeds
4. **Signal Order**: Children receive SIGTERM before parent exits

14.4. Testing
-------------

A comprehensive test suite validates:

- Parent tracks child PIDs correctly
- SIGTERM propagates from parent to registered children
- The server exits cleanly without SIGKILL-only shutdown logic
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
