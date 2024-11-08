# Abstract, HTTP/1.2, HTTP/3

Currently Google-Engineers plan pushing HTTP/3. As HTTP/2 still is not used commonly worldwide and
major implementation flaws, even performance issues exist we try to build a much simpler and secure
solution: **HTTP/1.2**.

## 1. Build / Test

See [BUILD.md](BUILD.md) to compile, tests: [/test/README.md](/test/README.md), specs: [/specs/md/README.md](/specs/md/README.md).

CI generated doc: [/doc/README.md](/doc/README.md) or http://docs.webcodex.de/wap.

## 2. Features

It will provide the following features:

- Much more leightweight
- Much more secure
- Much faster
- Drop bloating features
- Logical separated (TCP ports)
- Easy to implement
- Low cost maintainable
- Application Server friendly
- Proxy Server friendly
- Scalable to infinity

Also the part "HyperText" or even a complete renaming could be thought over. Its representation is by far not
more appropriate these days.

We will publish detailed RFP / RFC in the near future on http://der-it-pruefer.de.

A detailed feature comparison see: [HTTP Feature Matrix](FEATURE-MATRIX.md).

## 3. Primary Milestone

The primary projects goal is to provide a lightning-fast, rock solid and secure Python Application Server component
especially adapted for Modern-Browser-Applications (SPA).

Also the FalconAS Python Application Server will be a main-component of the [x0 JavaScript Framework](https://github.com/WEBcodeX1/x0) in the near future.

## 4. HTTP/2, HTTP/3

### 4.1. HTTP/2 Complexity

HTTP/2 adds too much complexity. The HTTP/1.1 simplicity is lost completely, especially OpenSource libraries
tend to be confusing and ununderstandable even using libevent.

Its everything packed in one really big box. Logical separation? Security? All going unfiltered over a single
TCP port. Ah, just use your IPS correctly! Bam: IPS process killed by 0day inside TCP packet.

Also adding TLS / SSL handling inside the protocol is not appropriate anymore. Let this handle a separated
component like nginx (reverse proxy), ingress-nginx on Kubernetes, stunnel or a Hardware-Loadbalancer.

Do not duplicate things you are not familiar with especially when other products exist which do the same
for decades much smarter.

### 4.2. HTTP/1.1 Pipelined Connections

HTTP/2 fixes the HTTP/1.1 Pipelined Connections problem. Due to a bug in asynchronously handling the responses
(wrong Request / Reply order) the complete feature was removed from *ALL* browsers worldwide.

Our HTTP/1.2 implementation also fixes the problem by extending the HTTP/1.1 protocol by just some lines of code.
A unique identifier (UUID) will be added for each Request so Response-Ordering will be obsolete.

>[!CAUTION]
> Our HTTP/1.2 implementation makes HTTP/2 and HTTP/3 obsolete and reduces complexity by far.

### 4.3. Thoughts about HTTP/3

HTTP/3 has been implemented using UDP, switching from TCP/IP. A possible solution for precalculated (non-realtime) CDN data, not realtime-applications.

Details at **Der IT Prüfer** / [Exemplary HTTP Network Processing](http://der-it-pruefer.de/).

>[!NOTE]
> All rumours telling HTTP/2 streams can get blocked by TCP/IP retransmissions is a hoax! If something gets blocked its caused by something in the OSI upper Layers, not Layer-3.

# 5. Proof Of Concept

We provide Proof Of Conecept Code and our working "FalconAS" HTTP/1.2 Server including these components:

- Static-Content-Server Component
- Applicaton-Server Component using Python Scripting Language

The Proxy-Server Component has been engineered and will be published later on.

# 6. Libevent / Epoll / Coroutines

Currently Coroutines is the word of interest in the Coding-Community. Libevent uses Coroutines.
Multiple Server Products use Libevent for client connection handling. 

But **beware**: Coroutines could be contraproductive for scaling aspects. If you are not familiar with
Kernel Socket Programming and implement the socket in a blocking way: Each client filedescriptor will be
monitored by the kernel separetely: *Welcome to the 90s*.

Concrete: the main loop in user space iterates over **ALL** current connection filedescriptors. If we have
10000 active or sleeping Keep-Alive connections: 10000 syscalls (context switches between user and kernel
space) will be done to check if socket contains received data. The kernel will additionally loop on 10000
filedescriptors checking if there is waiting incoming data. This will kill the kernels responsiveness.

Unlike (e.g. nginX) using epoll (also HTTP/1.2 uses epoll) a single syscall will tell user space which
filedescriptors have waiting data. With optimized 64bit server CPU and a good implementation: go lightspeed.

>[!NOTE]
> Coroutines in C++ should be used for Generators exclusively. Think of the Python Generators-Hype generating HTML code which has been dropped very very fast.
