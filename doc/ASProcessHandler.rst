7. AS Server Process Handler
============================

The application-server process handler currently provides the public lifecycle and configuration
hooks for backend workers.

.. note::

   The current implementation only contains placeholders for the new SHMVector system. This generic approach will simplify handling and implementation within multi-virtual-domain and multi-interpreter environments..

The active source code exposes:

* ``getASInterpreterCount()`` for counting configured interpreters
* ``setTerminationHandler()`` and ``terminate()`` for lifecycle control
* ``registerChildPID()`` to hand child PIDs back to ``Server``
* ``forkProcessASHandler()`` as the entry point for backend worker setup

7.1. Program Logic
------------------

7.1.1. Initialization
~~~~~~~~~~~~~~~~~~~~~

.. code-block:: text

   * Disable SIGINT / SIGPIPE for the backend entry point
   * Receive the SHM pointer bundle
   * Iterate over ConfigRef.Namespaces
   * Keep the child-worker fork block disabled while the runtime simplification is in progress

Workflow diagram see: :doc:`Graphical-Workflows` Section 15.1.1.

7.1.2. Main Loop
~~~~~~~~~~~~~~~~

.. code-block:: text

   * getASInterpreterCount() sums the configured "interpreters" values
   * The SIGTERM handler flips the internal RunServer flag to false
   * Shared-memory metadata and worker-loop code remain part of the interface surface,
     but the current implementation keeps the old child-process execution path commented out

Workflow diagram see: :doc:`Graphical-Workflows` Section 15.1.2.
