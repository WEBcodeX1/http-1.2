4. Main Server
==============

The main server process initializes all runtime components from the JSON configuration and then
enters a non-blocking poll loop.

4.1. Program Logic
------------------

4.1.1. Initialization
~~~~~~~~~~~~~~~~~~~~~

* Call ``setupSharedMemory()`` (currently a placeholder hook)
* Load and map static filesystem data via ``Configuration::mapStaticFSData()``
* Disable Signals / Setup Termination Handling
* Setup Server Listening Socket / Poll for Server Socket File Descriptor
* Drop System Privileges
* Enter Server Loop

Workflow diagram see: :doc:`Graphical-Workflows` Section 15.3.1.

4.1.2. Main Loop
~~~~~~~~~~~~~~~~

.. code-block:: text

   - While RunServer is true
     - Poll the server socket for new connections
     - On POLLIN: accept the client and register it in ClientHandler
     - On idle and no processed clients: sleep for IDLE_SLEEP_MICROSECONDS
     - Call processClients() every loop iteration

Workflow diagram see: :doc:`Graphical-Workflows` Section 15.3.2.
