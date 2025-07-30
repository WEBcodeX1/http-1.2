[![CodeQL](https://github.com/WEBcodeX1/http-1.2/actions/workflows/github-code-scanning/codeql/badge.svg)](https://github.com/WEBcodeX1/http-1.2/actions/workflows/github-code-scanning/codeql)
# Abstract: HTTP/1.2, HTTP/2 and HTTP/3

Currently, Google engineers are planning to push HTTP/3. As HTTP/2 is still
not widely adopted globally and has major implementation flaws, including
performance issues, we aim to build a much simpler and more secure solution:
**HTTP/1.2**.

## 1. Build / Test

Refer to [BUILD.md](BUILD.md) for compilation instructions, [test documentation](/test/README.md)
for testing, and [specifications documentation](/specs/md/README.md) for specs.

CI-generated documentation is available at [/doc/README.md](/doc/README.md) or
[http://docs.webcodex.de/wacp/rfp/](http://docs.webcodex.de/wacp/rfp/).

## 2. Features

- Lightweight
- Secure
- Fast
- Avoids bloated features
- Logical separation (TCP ports)
- Easy to implement
- Low-cost maintenance
- Application server-friendly
- Proxy server-friendly
- Infinitely scalable

Additionally, reconsidering the term "HyperText" or even renaming it might be
appropriate, as its representation is no longer accurate today.

Detailed RFP/RFC documentation will be published in the near future on
[http://der-it-pruefer.de](http://der-it-pruefer.de).

For a detailed feature comparison, see: [HTTP Feature Matrix](FEATURE-MATRIX.md).

## 3. Primary Milestone

The primary goal of this project is to provide a lightning-fast, rock-solid, and secure
Python application server component, specifically adapted for modern browser applications (SPA).

The FalconAS Python application server will also serve as a key component of the
[x0 JavaScript Framework](https://github.com/WEBcodeX1/x0) in the near future.

## 4. HTTP/2 and HTTP/3

### 4.1. HTTP/2 Complexity

HTTP/2 introduces excessive complexity. The simplicity of HTTP/1.1 is completely
lost. Open-source libraries are often confusing and difficult to understand, even
when using libevent.

Everything is packed into one large box, lacking logical separation and security.
All traffic is transmitted over a single TCP port, which poses significant risks.
For instance, an IPS process can be killed by a zero-day vulnerability inside a
TCP packet.

Additionally, incorporating TLS/SSL handling directly into the protocol is no
longer appropriate. Instead, this should be handled by separate components like
nginx (reverse proxy), ingress-nginx on Kubernetes, stunnel, or a hardware
load balancer.

Avoid duplicating functionality when better solutions have existed for decades.

### 4.2. HTTP/1.1 Pipelined Connections

HTTP/2 resolves the issue of HTTP/1.1 pipelined connections. However, due to a
bug in asynchronous response handling (misordered request/reply pairs), this feature
was removed from *all* browsers worldwide.

Our HTTP/1.2 implementation fixes this issue by extending the HTTP/1.1 protocol with minimal
changes. Specifically, a unique identifier (UUID) is added to each request, making response
ordering unnecessary.

> **Caution**
> Our HTTP/1.2 implementation renders HTTP/2 and HTTP/3 obsolete, drastically
> reducing complexity.

### 4.3. Thoughts on HTTP/3

HTTP/3 is implemented using UDP instead of TCP/IP. While this might work for precalculated
(non-realtime) CDN data, it is unsuitable for real-time applications.

Details are available at **Der IT Prüfer** / [Exemplary (HTTP) Network Processing](http://der-it-pruefer.de/).

> **Note**
> Claims that HTTP/2 streams can be blocked by TCP/IP retransmissions are false. Any
> blocking issues arise from the OSI upper layers, not Layer-3.

## 5. Proof of Concept

We provide proof-of-concept code and a working "FalconAS" HTTP/1.2 server, which
includes the following components:

- Static content server
- Application server using Python scripting

The proxy server component has been designed and will be published later.

## 6. Libevent / Epoll / Coroutines

Coroutines are currently a trending topic in the coding community. Libevent uses
coroutines, and many server products rely on Libevent for client connection handling.

However, **beware**: Coroutines can be counterproductive for scalability. Without
expertise in kernel socket programming, blocking socket implementations can result
in performance bottlenecks. Each client file descriptor is monitored separately by
the kernel, leading to inefficiencies reminiscent of the 1990s.

Specifically, the main loop in user space iterates over **all** active or idle
connection file descriptors. For example, 10,000 active or idle keep-alive
connections require 10,000 syscalls (context switches between user and kernel
space) to check for received data.

In contrast, technologies like nginx and our HTTP/1.2 implementation use epoll.
A single syscall informs user space about multiple file descriptors with pending
data. With optimized 64-bit server CPUs and a well-designed implementation, this
approach achieves lightning-fast performance.
