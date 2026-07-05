Main Server
===========

The main server process initializes all runtime components from the JSON configuration and then
enters a non-blocking poll loop.

Program Logic
-------------

Initialization
~~~~~~~~~~~~~~

* Call ``setupSharedMemory()`` (currently a placeholder hook)
* Load and map static filesystem data via ``Configuration::mapStaticFSData()``
* Disable Signals / Setup Termination Handling
* Setup Server Listening Socket / Poll for Server Socket File Descriptor
* Drop System Privileges
* Enter Server Loop

Workflow diagram see: :ref:`main-server-initialization-workflow`.

Main Loop
~~~~~~~~~

.. code-block:: text

   - While RunServer is true
     - Poll the server socket for new connections
     - On POLLIN: accept the client and register it in ClientHandler
     - On idle and no processed clients: sleep for IDLE_SLEEP_MICROSECONDS
     - Call processClients() every loop iteration

Workflow diagram see: :ref:`main-server-loop-workflow`.
