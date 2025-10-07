9. ResultScheduler
==================

The ResultScheduler component manages the following things "inside" ResultProcessor:

* Request "queuing"
* Result ordering for HTTP/1.1 requests

9.1. ResultProcessor Handling
-----------------------------

.. code-block:: text

   * On StaticFS Data
     - ResultOrder.append(ClientFD, HTTPType, ReqNr, SendFileFD)
   * On POST AS Data
     - ResultOrder.append(ClientFD, HTTPType, ReqNr, ASIndex)

   * Process (loop on "Requests" ClientFDs)
     - If ClientFD not in "LastRequest"
       - Insert into "LastRequest" with Default RequestNr 1 and RequestTime now()
     - ResultOrder.getNextRequests(ClientFD, HTTP1.2) -> ThreadHandler(getVirtualDomain())
     - ResultOrder.getNextRequests(ClientFD, HTTP1.1) -> ThreadHandler(getVirtualDomain())

     # Process in serial order. First: HTTP/1.2, afterwards HTTP/1.1.
       If no HTTP/1.2 request: process HTTP/1.1.

   * On ThreadHandler::Terminator::ClientFD::Termination (Callback pointer)
     - Update "LastRequest" (unordered map) ClientFD RequestNr, RequestTime
     - Remove all processed ClientFD ReqNrs from "Requests", "LastRequests"


9.2. Future Releases
--------------------

The development process showed: it is not advisable to implement `RequestUUID` into HTTP/1.1
compatibility. It never will be implemented into future browser code.

.. note::

   Future releases will offer a seperate **build option** for HTTP/1.1 and HTTP/1.2 making
   the ResultProcessor separation / ResultOrdering for HTTP/1.2 obsolete.
