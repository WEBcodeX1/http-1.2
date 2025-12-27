1. Preface
==========

Our intention is to build good, stable and secure software. This implies detailed documentation.

.. note::

   Do not forget: keep the clean code paradigm in mind.

The following document describes the Base-Layout of ``FalconAS`` / ``HTTP/1.2``, its Base Components
and Infrastructural Layout.

2. Logical Components
=====================

The following components are referenced throughout the documentation.

.. note::

   Logical Component Names used in documentation and the C++ Abstraction Model sometimes do not match.

2.1. Configuration
------------------

XML Configuration Handling. C++ Transformation.

2.2. Main::Server
-----------------

Main Server Loop.

:doc:`Main-Server`

2.3. Main::ClientHandler
------------------------

Client / Connection Handler.

:doc:`ClientHandler`

2.4. Main::StaticFSHandler
--------------------------

Static Files / Virtual Host Handler.

:doc:`StaticFSHandler`

2.5. ASProcessHandler
---------------------

Python Application Server Process Handler.

:doc:`ASProcessHandler`

2.6. ResultProcessor
--------------------

Result processing process (fork).

:doc:`ResultProcessor`

2.7. ResultProcessor::ThreadHandler
-----------------------------------

Result Thread Distributor.

Due to looping on Result sendfile(), write() and Header Processing, each ClientFDs requests (plural)
will be processed by a single POSIX Thread.

This also guarantees that multiple repeated result writes (in case of congestion) always will be
processed in serial order.

Big results using multiple writes are sent in the correct order as well.

2.8. ResultProcessor::ResultOrder
---------------------------------

Result Ordering will be done for the following types:

* Static Files, sendfile()
* Application Server Results (JSON), write()

2.8.1. HTTP/1.1
~~~~~~~~~~~~~~~

Result Ordering. Results must be sent in the exact order how they arrived.
The Result Ordering Class keeps track of dependent ClientFD Requests / Request Nr. and related.

2.8.2. HTTP/1.2
~~~~~~~~~~~~~~~

No order needed. Our implementation appends the "Request-UUID" HTTP Header to the HTTP Response.

3. Shared Memory Layout
=======================

Due to the Python Global Interpreter Lock the ``Main Server``, ``ResultProcessor`` and each Application
Server Process are implemented using Unix Processes.

Data Sharing (Requests, Synchronization) is done by Shared Memory and User Space Atomic Locks
(Kernel Mutex less).

.. code-block:: text

   +---------------------+---------------------+---------------------+---------------------+
   | Server Process      | AS Process 1        | AS Process x        | Result Processor    |
   |                     | Python Interpreter  | Python Interpreter  |                     |
   +---------------------+---------------------+---------------------+---------------------+
   | Shared Mem                                                                            |
   |  - StaticFS Requests                                                                  |
   |  - AS Metadata                                                                        |
   |  - AS Requests                                                                        |
   |  - AS Results                                                                         |
   +---------------------------------------------------------------------------------------+


.. note::

   32-bit memory addresses used for simplicity.

3.1. StaticFS Request SHM #1
----------------------------

.. code-block:: text

   Address                 Type                Descr           Default

   0x00000000              atomic_uint16_t     StaticFSLock    0
   0x00000002              uint16_t            RequestCount    0

   -- Req 1 Metadata ---------------------------------------------------

   0x00000004              uint16_t            ClientFD        Nullptr
   0x00000006              uint16_t            HTTPVersion     Nullptr
   0x00000008              uint16_t            RequestNr       Nullptr
   0x0000000a              uint16_t            PayloadLength   Nullptr
   0x0000000c              char[]              char[LenReq1]   Nullptr

   -- Req 2 Metadata ---------------------------------------------------

   0x0000000c+LenReq1      uint16_t            ClientFD        Nullptr
   0x0000000e+LenReq1+2    uint16_t            HTTPVersion     Nullptr
   0x00000010+LenReq1+4    uint16_t            RequestNr       Nullptr
   0x00000012+LenReq1+6    uint16_t            PayloadLength   Nullptr
   0x00000014+LenReq1+8    char[]              char[LenReq2]   Nullptr


3.2. AS Request / Result SHM #2
-------------------------------

.. code-block:: text

   Address                 Type                Descr           Default

   -- AS 1 Metadata ---------------------------------------------------

   0x00000000              atomic_uint16_t     CanRead         0
   0x00000002              atomic_uint16_t     WriteReady      0

   0x00000004              uint16_t            ClientFD        Nullptr
   0x00000006              uint16_t            HTTPVersion     1
   0x00000008              uint16_t            HTTPMethod      1
   0x0000000a              uint16_t            ReqNr           1
   0x0000000c              uint32_t            ReqPayloadLen   Nullptr
   0x00000010              uint32_t            ResPayloadLen   Nullptr

   -- AS 2 Metadata ---------------------------------------------------

   0x00000014              atomic_uint16_t     CanRead         0
   0x00000016              atomic_uint16_t     WriteReady      0

   0x00000018              uint16_t            ClientFD        Nullptr
   0x0000001a              uint16_t            HTTPVersion     1
   0x0000001c              uint16_t            HTTPMethod      1
   0x0000001e              uint16_t            ReqNr           1
   0x00000020              uint32_t            ReqPayloadLen   Nullptr
   0x00000024              uint32_t            ResPayloadLen   Nullptr


3.3. AS Request Payload SHM #3
------------------------------

.. code-block:: text

   Address                 Type                Descr           Default

   -- AS 1 Payload -----------------------------------------------------

   0x00000000              char[]              char[LenReq]    Nullptr

   -- AS 2 Payload -----------------------------------------------------

   0x00000000+SegmentSize  char[]              char[LenReq]    Nullptr


3.4. AS Result Payload SHM #4
-----------------------------

.. code-block:: text

   -- AS 1 Payload -----------------------------------------------------

   0x00000000              char[]              char[LenRes]    Nullptr

   -- AS 2 Payload -----------------------------------------------------

   0x00000000+SegmentSize  char[]              char[LenRes]    Nullptr
