1. Preface
==========

The current ``FalconAS`` / ``HTTP/1.2`` tree is centered around a small HTTP/1.1 runtime, a reusable
HTTP helper library, and supporting utilities for filesystem access and shared-memory-safe containers.

.. note::

   The implementation has been simplified compared to earlier multi-process drafts. This overview
   reflects the code that is currently present in ``src`` and ``lib/http``.

2. Logical Components
=====================

2.1. Configuration
------------------

JSON configuration loading and transformation into C++ runtime objects.

:doc:`Configuration`

2.2. Main::Server
-----------------

The top-level runtime that initializes the process, maps static filesystem data, sets up the
listening socket, and drives the main poll loop.

:doc:`Main-Server`

2.3. Main::ClientHandler
------------------------

The epoll-based client connection manager. It owns the active client map, reusable receive buffers,
and delegates socket reads to ``Client`` objects.

:doc:`ClientHandler`

2.4. Main::StaticFSHandler
--------------------------

Static filesystem indexing is provided by the ``Filesystem`` class and initialized through the
configuration layer at server startup.

:doc:`StaticFSHandler`

2.5. ASProcessHandler
---------------------

Backend process lifecycle hooks and interpreter-count discovery. The public interface for backend
child processes still exists, while the current source keeps the former worker-fork implementation
as scaffolding during the runtime simplification.

:doc:`ASProcessHandler`

2.6. HTTPLib::HTTPParser
------------------------

Incremental HTTP/1.1 request parsing for GET and POST requests, including header parsing, URL
parameter extraction, and partial POST-body handling.

:doc:`HTTPLib`

2.7. HTTPLib::HTTPMessageGenerator
----------------------------------

HTTP response message generation implemented by the ``HTTPGenerator`` class. It builds the response
status line, headers, body metadata, and incremental send state.

:doc:`HTTPLib`

2.8. SHMVector
--------------

``src/SHMVector.hpp`` provides a shared-memory-friendly vector implementation with contiguous
segment-based storage and atomic spinlock synchronization.

:doc:`SHMVector`

3. Runtime Layout
=================

The current runtime is simpler than the older documentation variants:

* ``Server::setupSharedMemory()`` is currently a placeholder hook.
* ``Client`` inherits from ``HTTPParser``, so request parsing happens per connection.
* Static filesystem data is mapped up front via ``Configuration::mapStaticFSData()``.
* The older dedicated result-processing pipeline is no longer part of the active
  documentation set.
* ``ASProcessHandler`` still exposes shared-memory pointer types and lifecycle methods, but the
  active source tree focuses on the server loop, client handling, HTTPLib, and ``SHMVector``.
