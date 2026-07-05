5. Client Handler
=================

The ``ClientHandler`` component owns the client socket registry, epoll state, and reusable
receive buffers used by ``Client`` objects.

5.1. Program Logic
------------------

5.1.1. Add Client
~~~~~~~~~~~~~~~~~

Called from Main::Server on new Client Socket.

* Make the accepted socket non-blocking
* Create a ``Client`` instance with the file descriptor and a reusable buffer segment
* Insert the client object into the internal map
* Add the file descriptor to epoll using ``EPOLLIN | EPOLLET``

Workflow diagram see: :doc:`Graphical-Workflows` Section 15.2.1.

5.1.2. Process Clients
~~~~~~~~~~~~~~~~~~~~~~

Get all Epoll FDs with waiting data.

.. note::

   If 5000 current Clients are connected and 2000 have waiting buffered data, we get an
   array of these 2000 file descriptor integers.

Reset ``ProcessedClients``, call ``epoll_wait()``, and pass the ready descriptors to
``readClientData()`` when data is available.

Workflow diagram see: :doc:`Graphical-Workflows` Section 15.2.2.

5.1.3. Read Client Data
~~~~~~~~~~~~~~~~~~~~~~~

Process all file descriptors with waiting data.

* Iterate over each ready epoll entry
* Look up the client in the active client map
* Call ``Client::receiveData()``
* On EOF or a hard receive error, erase the client entry and close the socket

The current source keeps request buffering and HTTP parsing on each ``Client`` instance and no
longer routes the read path through a separate result-processing stage.

Workflow diagram see: :doc:`Graphical-Workflows` Section 15.2.3.
