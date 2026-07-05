Client Handler Workflows
========================

This document contains Mermaid diagrams for the current ``ClientHandler`` implementation.

.. _client-handler-add-client-workflow:

Add Client
----------

.. mermaid::

    flowchart TD;
    A[addClient ClientFD] --> B[Set Socket Non-blocking];
    B --> C[Create Client Object];
    C --> D[Insert Client to Map];
    D --> E[Setup Epoll Event EPOLLIN,EPOLLET];
    E --> F[Add FD to Epoll];

.. _client-handler-process-clients-workflow:

Process Clients
---------------

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

.. _client-handler-read-client-data-workflow:

Read Client Data
----------------

.. mermaid::

    flowchart TD;
    A[readClientData FDCount] --> B[Loop through ready FDs];
    B --> C{More FDs?};
    C -->|Yes| D[Read FD from epoll event];
    D --> E{Client exists in map?};
    E -->|No| C;
    E -->|Yes| F[Call Client.receiveData];
    F --> G{Receive finished or hard error?};
    G -->|Yes| H[Erase client from map];
    H --> I[Close FD];
    I --> C;
    G -->|No| C;
    C -->|No| J[Return];
