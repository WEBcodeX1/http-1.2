15. Graphical Workflows
=======================

This document contains Mermaid diagrams that reflect the current implementation status in ``src``
and ``lib/http``.

15.1. Application Server Process Handler
----------------------------------------

15.1.1. ASProcessHandler Initialization
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. mermaid::

    flowchart TD;
    A[forkProcessASHandler] --> B[Disable SIGINT and SIGPIPE];
    B --> C[Receive SHM pointer bundle];
    C --> D[Iterate configured namespaces];
    D --> E[Current source keeps worker-fork block disabled];
    E --> F[Return to caller];


15.1.2. ASProcessHandler Runtime Hooks
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. mermaid::

    flowchart TD;
    A[getASInterpreterCount] --> B[Iterate ConfigRef.Namespaces];
    B --> C[Read namespace interpreters value];
    C --> D[Accumulate total];
    D --> E[Return count];
    F[SIGTERM] --> G[ASProcessHandler::terminate];
    G --> H[Set RunServer = false];


15.2. Client Handler
--------------------

15.2.1. Add Client
~~~~~~~~~~~~~~~~~~

.. mermaid::

    flowchart TD;
    A[addClient ClientFD] --> B[Set Socket Non-blocking];
    B --> C[Create Client Object];
    C --> D[Insert Client to Map];
    D --> E[Setup Epoll Event EPOLLIN,EPOLLET];
    E --> F[Add FD to Epoll];


15.2.2. Process Clients
~~~~~~~~~~~~~~~~~~~~~~~

.. mermaid::

    flowchart TD;
    A[processClients] --> B[Reset ProcessedClients = 0];
    B --> C[epoll_wait for ready FDs];
    C --> D{Error?};
    D -->|Yes| E[Log error and return];
    D -->|No| F{FDCount > 0?};
    F -->|Yes| G[readClientData FDCount];
    F -->|No| H[Return];
    G --> H;


15.2.3. Read Client Data
~~~~~~~~~~~~~~~~~~~~~~~~

.. mermaid::

    flowchart TD;
    A[readClientData FDCount] --> B[Loop through ready FDs];
    B --> C{More FDs?};
    C -->|Yes| D[Read FD from epoll event];
    D --> E{Client exists in map?};
    E -->|No| C;
    E -->|Yes| F[Call Client.receiveData()];
    F --> G{Receive finished or hard error?};
    G -->|Yes| H[Erase client from map];
    H --> I[Close FD];
    I --> C;
    G -->|No| C;
    C -->|No| J[Return];


15.3. Main Server
-----------------

15.3.1. Server Initialization
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. mermaid::

    flowchart TD;
    A[Server::init] --> B[Call setupSharedMemory hook];
    B --> C[Init Static Filesystem];
    C --> D[Configure Socket Address/Port];
    D --> E[Disable OS Signals SIGINT, SIGPIPE];
    E --> F[Setup Termination Handler];
    F --> G[Setup Server Socket];
    G --> H[Setup Poll for Server Socket];
    H --> I[Drop System Privileges];
    I --> J[Enter ServerLoop];


15.3.2. ServerLoop
~~~~~~~~~~~~~~~~~~

.. mermaid::

    flowchart TD;
    A[ServerLoop Start] --> B{RunServer == True?};
    B -->|Yes| C[Poll Server FD for Connections];
    C --> D{Poll Error?};
    D -->|Yes| E[Log Error];
    E --> B;
    D -->|No| F{POLLIN Event?};
    F -->|Yes| G[acceptClient];
    G --> H[Add Client to ClientHandler];
    H --> I[processClients];
    I --> B;
    F -->|No| J{ProcessedClients == 0?};
    J -->|Yes| K[Sleep IDLE_SLEEP_MICROSECONDS];
    K --> I;
    J -->|No| I;
    B -->|No| L[Server Exit];

15.4. HTTPLib::HTTPParser
-------------------------

15.4.1. appendBuffer()
~~~~~~~~~~~~~~~~~~~~~~

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


15.4.2. Request Processing
~~~~~~~~~~~~~~~~~~~~~~~~~~

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


15.5. HTTPLib::HTTPMessageGenerator
-----------------------------------

15.5.1. Message Lifecycle
~~~~~~~~~~~~~~~~~~~~~~~~~

.. mermaid::

    flowchart TD;
    A[MsgReset] --> B[MsgSetStatus];
    B --> C[MsgAddHeader / MsgAddDateHeader];
    C --> D[MsgSetBodyRef];
    D --> E[MsgGenerate];
    E --> F[Header buffer and body metadata ready];


15.5.2. Incremental Send Flow
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. mermaid::

    flowchart TD;
    A[MsgGetSendMetadata] --> B{Sending header or body?};
    B -->|Header| C[Return header pointer and remaining bytes];
    B -->|Body| D[Return body pointer and remaining bytes];
    C --> E[write/send bytes];
    D --> E;
    E --> F[MsgUpdateSendMetadata];
    F --> G{Header complete?};
    G -->|Yes| H[Switch to body];
    G -->|No| I[Continue sending header];
    H --> J{Body complete?};
    J -->|Yes| K[Transmission done];
    J -->|No| L[Continue sending body];
