Main Server Workflows
=====================

This document contains Mermaid diagrams for the current main server implementation.

.. _main-server-initialization-workflow:

Server Initialization
---------------------

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

.. _main-server-loop-workflow:

ServerLoop
----------

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
