HTTPParser Workflows
====================

This document contains Mermaid diagrams for the current ``HTTPParser`` implementation.

appendBuffer()
--------------

.. mermaid::

    flowchart TD;
    A[appendBuffer] --> B{Would buffer exceed limit?};
    B -->|Yes| C[Reject append];
    B -->|No| D[Append bytes to request buffer];
    D --> E{Waiting for POST body?};
    E -->|Yes| F{Enough bytes available?};
    F -->|Yes| G[Store payload and push request];
    F -->|No| H[Wait for more bytes];
    E -->|No| I{Header end marker found?};
    I -->|Yes| J[Split and process requests];
    I -->|No| H;

Request Processing
------------------

.. mermaid::

    flowchart TD;
    A[Split buffered data by CRLF CRLF] --> B[Process each request candidate];
    B --> C[Parse request line];
    C --> D{HTTP/1.1 and GET or POST?};
    D -->|No| E[Reject request];
    D -->|Yes| F[Parse headers];
    F --> G{GET or POST?};
    G -->|GET| H[Parse URL parameters and store request];
    G -->|POST| I[Validate Content-Length];
    I --> J{Body available?};
    J -->|Yes| K[Store payload and request];
    J -->|No| L[Keep partial POST state];
