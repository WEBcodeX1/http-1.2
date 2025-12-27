11. HTTP Library - Network Processing
=====================================

HTTP Library, Network Component. Used primarily in integration tests.

11.1. Linux User Space Interface
--------------------------------

The User Space Interface for current Linux Kernels 6.x+ still integrates IP Socket Processing
like this (simple non-blocking data read example excerpt).

.. code-block:: c

   while (Parser.position != EndBytePos) {
       int rc = read(Socket, Buffer, BufferSize);
       errnr = errno;
       if (rc < 0 && errnr == EAGAIN) { //-repeat }
       if (rc < 0 && errnr != EAGAIN) { /* set error */ break; }
       if (rc == 0) { /* set error rc, close con */ break; }
       if (rc > 0 ) { append_buffer(); }
       if (timeout_check()) { /* set timeout info rc */ break; }
   }

This is sub-optimal code. To handle this complex code structure, we should use the mini-library
described in the next section.

11.2. Event Library
-------------------

The Event Library is a small library to handle simple function callbacks.

.. note::

    The library was intended to be used to simplify HTTP/1.1 processing, under circumstances
    it could be obsolete for parsing / generating HTTP/1.2 XML based messages.
