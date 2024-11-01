# Abstract, HTTP/1.2, HTTP/3

Currently Google-Engineers plan pushing HTTP/3. As HTTP/2 still is not used commonly worldwide and
major implementation flaws, even performance issues exist we try to build a much simpler and secure
solution: **HTTP/1.2**.

# Build / Test

See [BUILD.md](BUILD.md) to compile, test and run.

# Features

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

A detailed feature comparison see: [HTTP Feature Matrix, HTTP/3](FEATURE-MATRIX.md).

## Web-Application-Server / Python

The primary projects goal is to provide a lightning-fast, rock solid and secure Python Application Server component
specially adapted for Modern-Browser-Applications (SPA).

# HTTP/2, HTTP/3

## HTTP/2 Complexity

HTTP/2 adds too much complexity. The HTTP/1.1 simplicity is lost completely, especially OpenSource libraries
tend to be confusing and ununderstandable even using libevent.

Its everything packed in one really big box. Logical separation? Security? All going unfiltered over a single
TCP port. Ah, just use your IPS correctly! Bam: IPS process killed by 0day inside TCP packet.

Also adding TLS / SSL handling inside the protocol is not appropriate anymore. Let this handle a separated
component like nginx (reverse proxy), ingress-nginx on Kubernetes, stunnel or a Hardware-Loadbalancer.

Do not duplicate things you are not familiar with especially when other products exist which do the same
for decades much smarter.

## HTTP/1.1 Pipelined Connections

HTTP/2 fixes the HTTP/1.1 Pipelined Connections problem. Due to a bug in asynchronously handling the responses
(wrong Request / Reply order) the complete feature was removed from *ALL* browsers worldwide.

Our HTTP/1.2 implementation also fixes the problem by extending the HTTP/1.1 protocol by just some lines of code.
A unique identifier (UUID) will be added for each Request so Response-Ordering will be obsolete.

>[!CAUTION]
> Our HTTP/1.2 implementation makes HTTP/2 and HTTP/3 obsolete and reduces complexity by far.

## Thoughts about HTTP/3

HTTP/3 has been implemented using UDP, switching from TCP/IP.

>[!NOTE]
> All rumours telling HTTP/2 streams can get blocked by TCP/IP retransmissions is a hoax!

## Web Application Server

With fixing the Pipelined Connections problem, HTTP/2 **should** also fix speeding up dynamic WebServices
Request / Response times. In theory the HTTP/2 specs look quite good, real live shows different behaviour.

This could be caused by:

- Defective implementation inside the HTTP library
- Defective programming on Application-Server side
- Missing fine-grained Test-Framework

# Proof Of Concept

We provide Proof Of Conecept Code and our working "FalconAS" HTTP/1.2 Server including those components:

- Static-Content-Server Component
- Applicaton-Server Component using Python Scripting Language

The Proxy-Server Component has been engineered and will be published later on.

Detailed draft and specifications can be found under /draft and /specs subdirs.

# Libevent / Epoll / Coroutines

Currently Coroutines is the word of interest in the Coding-Community. Libevent uses Coroutines.
Multiple Server Products use Libevent for client connection handling. 

But **beware**: Coroutines could be contraproductive for scaling aspects. If you are not familiar with
Kernel Socket Programming and implement the socket in a blocking way: Each client filedescriptor will be
monitored by the kernel separetely. Welcome to the 90s.

Concrete: the main loop in user space iterates over ALL current connection filedescriptors. If we have
10000 active or sleeping Keep-Alive connections: 10000 syscalls (context switches between user and kernel
space) will be done. The kernel will additionally loop on 10000 filedescriptors checking if there is
waiting incoming data. This will kill the kernels responsiveness.

Unlike (also nginX) using epoll (also we use epoll) a single syscall will tell user space which
filedescriptors have waiting data. With optimized 64bit server CPU and good server code: Scaling, bingo!

