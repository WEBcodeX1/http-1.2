HTTPLib
===========

The HTTP library currently consists of two documented components:

* ``HTTPParser`` in ``lib/http/httpparser.*``
* ``HTTPMessageGenerator`` implemented by ``HTTPGenerator`` in ``lib/http/httpgenerator.*``

HTTPParser
----------------

``HTTPParser`` is an incremental HTTP/1.1 parser used directly by ``Client`` objects.

Current parser behavior:

* Accepts GET and POST requests
* Requires ``Content-Length`` for POST requests
* Supports multiple requests in a single receive buffer
* Keeps partial POST state until the configured body length has arrived
* Extracts request headers, URL, payload, and GET parameters into ``RequestProperties_t``
* Rejects unsupported protocol versions and malformed request lines

appendBuffer()
~~~~~~~~~~~~~~~~~~~~~~

``appendBuffer(const char*, const uint16_t)`` appends new socket bytes to the internal request
buffer until ``_HTTPRequestBufferMax`` is reached.

Processing flow:

1. Reject the append if the configured buffer limit would be exceeded
2. Append the received bytes to ``_HTTPRequestBuffer``
3. If the parser is waiting for a POST body and enough bytes are now available, complete the request
4. Otherwise, start request processing once the HTTP header end marker has been found

Request Splitting and Parsing
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

``_processRequests()`` and ``_processRequestProperties()`` split buffered data by ``\\r\\n\\r\\n``
and build ``RequestProperties_t`` entries.

The parser currently records:

* ``HTTPVersion``
* ``HTTPMethod``
* ``RequestHeaders``
* ``URL``
* ``Payload``
* ``URLParams``

Public Accessors
~~~~~~~~~~~~~~~~~~~~~~~~

``getRequests()`` returns the parsed request vector by value, while ``getRequestsPtr()`` returns a
pointer to the same internal request collection.

HTTPMessageGenerator
--------------------------

The documented message-generator component is implemented by the ``HTTPGenerator`` class.

Current generator behavior:

* Stores an HTTP status code and status text
* Adds arbitrary response headers
* Generates an RFC-style ``Date`` header through ``MsgAddDateHeader()``
* Tracks header/body send progress through ``SendMetadata_t``
* Supports incremental sending by updating the active buffer pointer after each write

Message Construction
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Typical usage:

1. ``MsgReset()``
2. ``MsgSetStatus(...)``
3. ``MsgAddHeader(...)`` and optionally ``MsgAddDateHeader()``
4. ``MsgSetBodyRef(...)``
5. ``MsgGenerate()``

``MsgGenerate()`` builds an ``HTTP/1.1`` status line, serializes all headers, appends
``Content-Length``, and prepares the header buffer for transmission.

Incremental Send State
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

``MsgGetSendMetadata()`` exposes the active buffer pointer and remaining byte count for either the
header or the body.

``MsgUpdateSendMetadata(ssize_t SentBytes)`` advances the active pointer after each send operation
and switches from header mode to body mode once the header has been transmitted completely.
