Client Handler
==============

The ``ClientHandler`` component owns the client socket registry, epoll state, and reusable
receive buffers used by ``Client`` objects.

Program Logic
-------------

Add Client
~~~~~~~~~~

Called from Main::Server on new Client Socket.

* Make the accepted socket non-blocking
* Create a ``Client`` instance with the file descriptor and a reusable buffer segment
* Insert the client object into the internal map
* Add the file descriptor to epoll using ``EPOLLIN | EPOLLET``

Workflow diagram see: :ref:`client-handler-add-client-workflow`.

Process Clients
~~~~~~~~~~~~~~~

Get all Epoll FDs with waiting data.

.. note::

   If 5,000 clients are currently connected and 2,000 have pending buffered data, the system outputs an array containing the 2,000 corresponding file descriptor integers.

Reset ``ProcessedClients``, call ``epoll_wait()``, and pass the ready descriptors to
``readClientData()`` when data is available.

Workflow diagram see: :ref:`client-handler-process-clients-workflow`.

Read Client Data
~~~~~~~~~~~~~~~~

Process all file descriptors with waiting data.

* Iterate over each ready epoll entry
* Look up the client in the active client map
* Call ``Client::receiveData()``
* On EOF or a hard receive error, erase the client entry and close the socket

The current source keeps request buffering and HTTP parsing on each ``Client`` instance and no
longer routes the read path through a separate result-processing stage.

Workflow diagram see: :ref:`client-handler-read-client-data-workflow`.
