Preface
=======

The current ``FalconAS`` / ``NLAP`` updates (`version 0.3`) target two core areas:

1. ``Microcontroller-Optimized HTTP/1.1``: A clean, secure library tailored for embedded environments.
2. ``NLAP Draft & Schemas``: Next Level Application Protocol specifications implementing a 100% non-blocking, scalable client-handling approach for ``TCP`` Sockets (compared to ``HTTP/2.0`` and ``HTTP/3.0``).

.. important::

   The implementation has been simplified compared to `version 0.1` and `version 0.2`. This overview reflects the code that
   is currently present in ``src`` and ``lib/http``.

The current implementation status is as following:

1. ``HTTP/1.0 Library``: Developed a robust HTTP/1.0 library containing HTTPParser and HTTPMessageGenerator. This library has been successfully tested on an ESP32-S3 microcontroller.
2. ``Client Handling / Data Processing``: Optimized receiving and result-sending functionality (see `ESP32-S3`_ code for reference). This optimized client data handling serves as the boilerplate for all upcoming XML-based NLAP sub-features.
3. ``Upcoming Milestones``: Development of a concurrent, multi-processing, and 100% non-blocking NLAP client / server library is underway. Furthermore, the data encryption process is engineered and scheduled for implementation.

.. _ESP32-S3: https://github.com/WEBcodeX1/micropython-as/tree/main/src/components/network_oop

Logical Components
==================

Configuration
-------------

JSON configuration loading and transformation into C++ runtime objects.

:doc:`Configuration`

Main::Server
------------

The top-level runtime that initializes the process, maps static filesystem data, sets up the
listening socket, and drives the main poll loop.

:doc:`Main-Server`

Main::ClientHandler
-------------------

The epoll-based client connection manager. It owns the active client map, reusable receive buffers,
and delegates socket reads to ``Client`` objects.

:doc:`ClientHandler`

Main::StaticFSHandler
---------------------

Static filesystem indexing is provided by the ``Filesystem`` class and initialized through the
configuration layer at server startup.

:doc:`StaticFSHandler`

ASProcessHandler
----------------

Backend process lifecycle hooks and interpreter-count discovery. The public interface for backend
child processes still exists, while the current source keeps the former worker-fork implementation
as scaffolding during the runtime simplification.

:doc:`ASProcessHandler`

HTTPLib::HTTPParser
-------------------

Incremental HTTP/1.1 request parsing for GET and POST requests, including header parsing, URL
parameter extraction, and partial POST-body handling.

:doc:`HTTPLib`

HTTPLib::HTTPMessageGenerator
-----------------------------

HTTP response message generation implemented by the ``HTTPGenerator`` class. It builds the response
status line, headers, body metadata, and incremental send state.

:doc:`HTTPLib`

SHMVector
---------

``src/SHMVector.hpp`` provides a shared-memory-friendly vector implementation with contiguous
segment-based storage and atomic spinlock synchronization.

:doc:`SHMVector`

Runtime Layout
==============

The current runtime is simpler than the older documentation variants:

* ``Server::setupSharedMemory()`` is currently a placeholder hook.
* ``Client`` inherits from ``HTTPParser``, so request parsing happens per connection.
* Static filesystem data is mapped up front via ``Configuration::mapStaticFSData()``.
* The older dedicated result-processing pipeline is no longer part of the active
  documentation set.
* ``ASProcessHandler`` still exposes shared-memory pointer types and lifecycle methods, but the
  active source tree focuses on the server loop, client handling, HTTPLib, and ``SHMVector``.
