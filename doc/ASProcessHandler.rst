7. Application Server Process Handler
=====================================

The Application Server Process Handler manages:

* Setup Application Server Processes / Instances (fork)
* SHM Requests / Synchronization for all AS Instances
* SHM Writes / Synchronization for all AS Instances

7.1. Program Logic
------------------

7.1.1. Initialization
~~~~~~~~~~~~~~~~~~~~~

.. code-block:: text

   ASIndex = 0
   * Iterate over Config::VirtualDomains
     - Iterate over VirtualDomain::ASCount
       - Fork AS[Index] Process / SHM Reference
       - Index++

Workflow diagram see: :doc:`Graphical-Workflows` Section 1.1.1.

7.1.2. Main Loop
~~~~~~~~~~~~~~~~

.. code-block:: text

   * Loop
     - Check CanRead == 1 && WriteReady == 0
       - If yes: Process Python Request
       - Write CanRead == 0 && WriteReady = 1

Workflow diagram see: :doc:`Graphical-Workflows` Section 1.1.2.
