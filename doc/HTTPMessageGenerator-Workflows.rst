HTTPMessageGenerator
====================

This document contains Mermaid diagrams for the current ``HTTPGenerator`` / message-generator implementation.

Message Lifecycle
-----------------

.. mermaid::

    flowchart TD;
    A[MsgReset] --> B[MsgSetStatus];
    B --> C[MsgAddHeader / MsgAddDateHeader];
    C --> D[MsgSetBodyRef];
    D --> E[MsgGenerate];
    E --> F[Header buffer and body metadata ready];

Incremental Send Flow
---------------------

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
