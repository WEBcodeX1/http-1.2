AS Process Handler
==================

This document contains Mermaid diagrams for the current ``ASProcessHandler`` implementation.

.. _as-process-handler-initialization-workflow:

ASProcessHandler Initialization
-------------------------------

.. mermaid::

    flowchart TD;
    A[forkProcessASHandler] --> B[Disable SIGINT and SIGPIPE];
    B --> C[Receive SHM pointer bundle];
    C --> D[Iterate configured namespaces];
    D --> E[Current source keeps worker-fork block disabled];
    E --> F[Return to caller];

.. _as-process-handler-runtime-workflow:

ASProcessHandler Runtime Hooks
------------------------------

.. mermaid::

    flowchart TD;
    A[getASInterpreterCount] --> B[Iterate ConfigRef.Namespaces];
    B --> C[Read namespace interpreters value];
    C --> D[Accumulate total];
    D --> E[Return count];
    F[SIGTERM] --> G[ASProcessHandler::terminate];
    G --> H[Set RunServer = false];
