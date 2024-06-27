# Abstract / HTTP1.2

Currently Google-Engineers plan moving towards HTTP3. As HTTP2 still is not used commonly worldwide and
major implementation flaws and performance issues exist we try to build a much simpler and secure
solution: **HTTP1.2**.

It will provide the following features:

- Much more lightweight
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

## Thoughts about HTTP3

HTTP3 should be implemented using UDP or even UDPLite. Phew, also it should be HTTP downwards compatible.

How could this be achieved by switching from a Connection-Oriented (TCP) to a much much simpler
Connection-Less-Protocol (UDP)? Maybe engineering department has contracts with Harry Potter?

Take a look at the following topics:

- Transport Layer Security
- Asynchronous Web Application Server Access
- Transparent Proxy Server Handling
- WebSockets / Chat
- Packet Retransmission / Timeouts

All logic which has been removed from transparent, 100% working Layer3 protocols must now be re-implemented
"By Hand" including packet retransmission into Layer7. Phew: every Browser, every Server, every HTTP-related
program has to implement this by themselves.

Much fun implementing! See more details on http://der-it-pruefer.de.

# HTTP2

## HTTP2.0 Complexity

HTTP2.0 adds too much complexity. The HTTP1.1 simplicity is lost completely, especially OpenSource libraries
tend to be confusing and ununderstandable even using libevent.

Its evertything packed in one really big box. Logical separation? Security? All going unfiltered over a single
TCP port. Ah, just use your IPS correctly! Bam: IPS process killed by 0day inside TCP packet.

Also adding TLS / SSL handling inside the protocol is not appropriate anymore. Let this handle a separated
component like Ingress-nginX on Kubernetes or a Hardware-Loadbalancer. Do not duplicate things you are not
familiar with especially when other products exist which do the same for decades much smarter.

## HTTP1.1 Pipelined Connections

HTTP2 fixes the HTTP1.1 Pipelined Connections problem. Due to a bug in asynchronously handling the responses
(wrong Request / Reply order) the complete feature was removed from *ALL* browsers worldwide.

Our HTTP1.2 implementation also fixes the problem by extending the HTTP1.1 protocol by just some lines of code.
A unique identifier (UUID) will be added for each Request so Response-Ordering will be obsolete.

## Web Application Server

With fixing the Pipelined Connections problem, HTTP2 **should** also fix speeding up dynamic WebServices
Request / Response times. In theory the HTTP2 specs look quite good, real live shows different behaviour.

This could be caused by:

- Defective implementation inside the HTTP library
- Defective programming on Application-Server side
- Missing fine-grained Test-Framework

# Proof Of Concept

We provide Proof Of Conecept Code and our working "FalconAS" HTTP1.2 Server including those components:

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
space) will be done *every main loop iteration*. The kernel will additionally loop on 10000 filedescriptors checking if there is
waiting incoming data. This will kill the kernels responsiveness.

Unlike (also nginX) using epoll (also we use epoll) a single syscall will tell user space which
filedescriptors have waiting data. With optimized 64bit server CPU and good server code: Scaling, bingo!

