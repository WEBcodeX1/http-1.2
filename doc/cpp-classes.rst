C++ Classes Reference
=====================

This section provides documentation for all C++ classes in the FalconAS HTTP/1.2 server.

Core Server Classes
-------------------

Server
~~~~~~

The main server class that orchestrates all server operations.

.. doxygenclass:: Server
   :members:
   :undoc-members:

Client
~~~~~~

Represents a client connection and manages client-specific state.

.. doxygenclass:: Client
   :members:
   :undoc-members:

ClientHandler
~~~~~~~~~~~~~

Handles client connections, epoll setup, and data buffering.

.. doxygenclass:: ClientHandler
   :members:
   :undoc-members:

Configuration Classes
---------------------

Configuration
~~~~~~~~~~~~~

Manages server configuration loaded from JSON configuration file.

.. doxygenclass:: Configuration
   :members:
   :undoc-members:

Filesystem
~~~~~~~~~~

Handles filesystem operations and virtual host file mappings.

.. doxygenclass:: Filesystem
   :members:
   :undoc-members:

Request Processing Classes
--------------------------

ASRequestHandler
~~~~~~~~~~~~~~~~

Handles application server requests.

.. doxygenclass:: ASRequestHandler
   :members:
   :undoc-members:

ASProcessHandler
~~~~~~~~~~~~~~~~

Manages Python application server processes.

.. doxygenclass:: ASProcessHandler
   :members:
   :undoc-members:

ResultProcessor
~~~~~~~~~~~~~~~

Processes and sends results to clients.

.. doxygenclass:: ResultProcessor
   :members:
   :undoc-members:

ResultOrder
~~~~~~~~~~~

Manages result ordering for HTTP/1.1 compliance.

.. doxygenclass:: ResultOrder
   :members:
   :undoc-members:

ThreadHandler
~~~~~~~~~~~~~

Manages thread distribution for result processing.

.. doxygenclass:: ThreadHandler
   :members:
   :undoc-members:

Memory Management Classes
-------------------------

MemoryManager
~~~~~~~~~~~~~

Template class for aligned memory management with huge page support.

.. doxygenclass:: MemoryManager
   :members:
   :undoc-members:

Shared Memory Classes
---------------------

SHMStaticFS
~~~~~~~~~~~

Shared memory handler for static file system requests.

.. doxygenclass:: SHMStaticFS
   :members:
   :undoc-members:

SHMPythonAS
~~~~~~~~~~~

Shared memory handler for Python application server communication.

.. doxygenclass:: SHMPythonAS
   :members:
   :undoc-members:

IPC Classes
-----------

IPCHandler
~~~~~~~~~~

Base class for inter-process communication.

.. doxygenclass:: IPCHandler
   :members:
   :undoc-members:

IPCHandlerAS
~~~~~~~~~~~~

IPC handler specific to application server processes.

.. doxygenclass:: IPCHandlerAS
   :members:
   :undoc-members:

Utility Classes
---------------

CPU
~~~

CPU affinity and binding utilities.

.. doxygenclass:: CPU
   :members:
   :undoc-members:

Vector
~~~~~~

Custom vector implementation with specialized operations.

.. doxygenclass:: Vector
   :members:
   :undoc-members:

Helper Classes
~~~~~~~~~~~~~~

Socket
^^^^^^

Socket utility functions.

.. doxygenclass:: Socket
   :members:
   :undoc-members:

FilesystemHelper
^^^^^^^^^^^^^^^^

Filesystem helper utilities.

.. doxygenclass:: FilesystemHelper
   :members:
   :undoc-members:

String
^^^^^^

String manipulation utilities.

.. doxygenclass:: String
   :members:
   :undoc-members:
