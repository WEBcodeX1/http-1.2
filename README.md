![FalconAS Logo](./image/falconas-logo.png)

Low Latency TCP - Falcon Application Server - NLAP (Next Level Application Protocol)

[![CodeQL](https://github.com/WEBcodeX1/http-1.2/actions/workflows/github-code-scanning/codeql/badge.svg)](https://github.com/WEBcodeX1/http-1.2/actions/workflows/github-code-scanning/codeql)
[![License](https://img.shields.io/github/license/WEBcodeX1/http-1.2?style=flat)](https://github.com/WEBcodeX1/http-1.2/tree/main/LICENSE)
[![Doxygen API](https://img.shields.io/badge/docs-doxygen-blue)](https://docs.webcodex.de/developer/falconas/v0.3/doxygen/index.html)
[![Sphinx Docs](https://img.shields.io/badge/docs-sphinx-blue)](https://docs.webcodex.de/developer/falconas/v0.3/sphinx/index.html)

---

# 1. Overview

The Next-Level Application Protocol (NLAP) Suite and its accompanying server middleware
(featuring a high-speed Python / Java backend) are engineered specifically for building
high-speed data-aggregation systems that require ultra-low-latency transmission outside
the browser scope.

As a low-level, TCP XML-message-framed transport framework, NLAP establishes a highly efficient
client / server protocol architecture. While tailored for standalone backend infrastructure,
the suite holds long-term potential for future browser integration to enhance web-application
performance.

Within this ecosystem, the high-speed application server relies on an optimized base architecture
and structural design to drive overall performance gains, natively utilizing **NLAP** as its core transport
mechanism.

# 1.1. Implementation Specifications

The implementation requirements / RFP adhere strictly to the guidelines detailed in the
[Network Sockets Insight](https://www.der-it-pruefer.de/network/Network-Sockets-Insight) article
from *Der IT Prüfer* BLOG. This ensures full compliance with the  outlined specifications.

# 1.2. Empirical Validation

Our current **ESP32-S3** PONG game project—a real-time, multiplayer, browser-controllable
game—proves that even a highly CPU-constrained device achieves outstanding performance when
powered by this framework's low-latency network sub-components, see [micropython-as](https://github.com/WEBcodeX1/micropython-as).

The inclusion of default **TCP AccECN** (Accurate Explicit Congestion Notification) in recent Linux
kernels validates that our choice of TCP—as utilized in this project—has always been the correct,
forward-looking architectural decision.

# 2. Project Evolution & History

The project was originally conceptualized under the designation `HTTP/1.2`. The initial
objective was to mitigate the limitations of the flawed `HTTP/1.1` pipelining specification
by injecting unique **UUIDs** into individual requests.

However, practical implementation demonstrated that this approach introduces severe
technical problems. 

Because the `HTTP/1.1` specification relies strictly on synchronous, serial processing,
it remains fundamentally incompatible with modern, deterministic zero-latency architectures.
Consequently, the `HTTP/1.2` pipelining methodology was deprecated in favor of a novel
architectural framework: **NLAP**.

# 3. What is NLAP? What problems does NLAP solve?

NLAP (Next Level Application Protocol) is a deterministic, transaction-oriented
transport framework that formally resolves long-standing architectural omissions
in Layer 5 (Session) and Layer 6 (Presentation) of the OSI model. Originally
conceptualized by IETF engineers as an in-kernel transactional framed protocol,
NLAP realizes this design paradigm in user space, utilizing standard `SOCK_STREAM`
sockets as a foundational base protocol.

By replacing traditional, continuous stream-based processing with discrete, strictly validated
XML message frames, NLAP achieves exceptional throughput, structural security, and minimal
latency.

**Core Architectural Characteristics:**

- **Strict XML Message Framing:** Eliminates stream-parsing ambiguities by processing strictly bounded data packets. This non-streamed approach significantly enhances parsing security, mitigates memory-corruption vectors, and maximizes raw processing performance.
- **Formalized Model Descriptions:** Reduces protocol complexity to a bare minimum by enforcing a 100% complete structural and semantic definition via Document Type Definitions (DTD) and YANG modeling schemas.
- **Granular Protocol Sub-typing:** Sub-divides transport traffic into distinct, functional protocol variants to maximize scalability and simplify network firewalls (see chapter [7. NLAP Subtypes](#7-nlap-subtypes)).
- **Simplified High-Integrity Cryptography**: Drastically simplifies cryptographic state-machines and maximizes security by eliminating partial stream-based encryption entirely. Implemented via WolfSSL, data is signed and encrypted atomically as discrete, static messages. This approach supports standard X.509 mechanisms natively across both high-performance OpenSC (x86_64) environments and embedded IoT deployments via direct hardware TPM integration.
- **End-to-End Non-Blocking Architecture:** Features non-blocking execution primitives across all protocol layers. This design integrates seamlessly with Linux Kernel 7.0 AccECN (Accurate ECN) to optimize TCP retransmission timeouts (RTO) and low-latency feedback loops.
- **Near-Kernel Latency & Zero HoL Blocking**: Inherently eliminates head-of-line (HoL) blocking over a single socket connection. By deploying hybridized io_uring and epoll I/O frameworks, NLAP achieves deterministic processing speeds that mirror kernel-level transport latencies.

# 4. Achievements

The technical progression and current state of the NLAP implementation comprise the following structural phases and components:

1. **Protocol Paradigm Validation:** Analytical evaluation of HTTP/1.1 pipeline extensions resulted in the complete deprecation of stream-oriented processing for the FalconAS architecture in favor of a transactional framework.
2. **I/O Subsystem Evaluation:** Systematic review of synchronous Berkeley Sockets and multi-threaded processing layouts identified critical architectural bottlenecks, leading to the rejection of traditional multi-threading paradigms.
3. **Reference Socket Specification:** Formulated and published a verified, non-blocking, and deterministic Berkeley Sockets blueprint on *Der IT Prüfer* ([Technical Insight](https://www.der-it-pruefer.de/network/Network-Sockets-Insight)).
4. **Cross-Platform Verification:** Demonstrated the portability of the socket layer by adapting the core FalconAS network-handling runtime to resource-constrained environments, utilizing the ESP32-S3 microcontroller as a reference platform.
5. **C++23 Parsing Library:** Engineered a specialized, performance- and heap-optimized C++23 validation library for low-level HTTP/1.1 parsing and message generation.
6. **Architectural Refactoring:** Executed a comprehensive code-base refactoring based on the empirical performance metrics gathered from the initial reference implementations.
7. **Schema Implementation:** Developed the comprehensive structural boundaries for all NLAP protocol subtypes, formalized through complete Document Type Definitions (DTD) and YANG modeling layouts compiled with AI assistance.
8. **Zero-Copy XML Parsing Engine:** Implemented a memory-optimized XML parsing layer utilizing C++23 features (`std::generator`) and non-allocating string views (`std::string_view`) to minimize data-handling overhead, developed with AI assistance.

# 5. Working Components

Below is a brief overview of the currently functional and production-ready components:

- **Memory (heap)-optimized HTTP/1.1 library:** Features a fast parser and message generator ([`/lib/http/`](/lib/http/)).
- **Microcontroller portability:** The HTTP/1.1 parser library is fully ported to the ESP-IDF based ESP32-S3 and ESP32-C3 platforms ([`/ports/arduino/`](/ports/arduino/)).
- **Structural specifications:** Includes core XML and workflow schemas formalized via DTD and YANG models ([`/specs/`](/specs/)).
- **Optimized XML processing:** A memory-optimized Apache Xerces-based parser tailored for NLAP validation and message processing.

# 6. Milestones

The following developments are scheduled for immediate implementation:

- **Kernel I/O Integration:** Adaptation of Linux `io_uring` (for asynchronous zero-copy receiving and sending) and `epoll` (restricted to transmission operations), adhering to the architectures validated in sections 4.2, 4.3, and 4.4.
- **NLAMP Prototype:** Initial development of the Application Server Metadata ("M") prototype, featuring process-based Shared Memory (SHM) isolation within *FalconAS*.
- **NLAFP Prototype:** Initial development of the File-Transport ("F") prototype, utilizing high-speed, direct user-space I/O.
- **NLAPS Integration:** Full architectural implementation of the ("S") Security Extensions layer.

# 7. NLAP Subtypes

Each NLAP sub-protocol operates on a dedicated TCP port and is structured as follows:

- **NLAFP:** File-transport operations.
- **NLAMP:** Application server Metadata (JSON) exchange.
- **NLAPP:** Upcoming Proxy Server implementation, featuring auto-scaling and multi-endpoint support.
- **NLAPS:** Security Extensions, handling cryptographic signing, encryption, and authentication.

Detailed specifications for each sub-protocol are available in the [`/specs/`](/specs/) directory.
