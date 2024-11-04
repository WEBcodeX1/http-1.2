# 1. Feature Matrix (Basic)

The following table shows features compared to the different HTTP Versions.

| Feature                             | HTTP/1.1                     | HTTP/1.2                                | HTTP/2                   |
|-------------------------------------|------------------------------|-----------------------------------------|--------------------------|
| Keep-Alive Connections              | :white_check_mark:           | :white_check_mark: (permanent, see 1.1) | :white_check_mark:       |
| Parametrized Keep-Alive (Header)    | :white_check_mark:           | :x: (see 1.2)                           | :white_check_mark:       |
| Transfer Encoding (Compressed)      | :white_check_mark:           | :x: (see 1.3)                           | :white_check_mark:       |
| Byte Range Requests                 | :white_check_mark:           | :x: (see 1.4)                           | :white_check_mark:       |
| Pipelined Requests                  | :white_check_mark: (broken)  | :white_check_mark: (working, see 1.5)   | :x: (Layer7 multiplexed) |
| Chunked Encoding                    | :white_check_mark:           | :x: (see 1.6)                           | :white_check_mark:       |
| Caching Mechanism / Header          | :white_check_mark:           | :white_check_mark: (Static Content)     | :white_check_mark:       |
| Caching 304 not modified            | :white_check_mark:           | :white_check_mark: (Static Content)     | :white_check_mark:       |
| Cookies                             | :white_check_mark:           | :white_check_mark:                      | :white_check_mark:       |
| Text Based Protocol                 | :white_check_mark:           | :white_check_mark:                      | :x:                      |
| Binary Based Protocol               | :x:                          | :x: (see 1.7)                           | :white_check_mark:       |

See [Exemplary HTTP Network Processing](http://der-it-pruefer.de/) for a detailed explanation / analysis why HTTP/2 and HTTP/3 are not suitable for future-proof Web-Applications.

## 1.1. Permanent Keep-Alive Connection

HTTP/1.2 uses *permanent* Keep-Alive. This means **1 Single Client** always connects through **1 Single Socket** to a
Server Domain / Virtual Host.

>[!TIP]
> TCP/IP **does** allow multiple "multiplexed" data-channels **by design** without blocking anything or arising retransmission problems.

>[!CAUTION]
> HTTP/1.2 will correct the broken HTTP/1.1 *Pipelined Connection* implementation. This will automagically use the existing rock solid TCP/IP data channel "multiplexing" feature (which was unnecessarily implemented on top into HTTP/2 at Layer7).

Also read [Exemplary HTTP Network Processing](http://der-it-pruefer.de/).

## 1.2. Parametrized Keep-Alive

Due to a permanent Keep-Alive, we also do not need Parametrized Keep-Alive settings which will drastically reduce
Protocol-Logic.

Also it is no good idea to allow a non-authenticated client to modify server-parameters (from a security point of view) at runtime.

## 1.3. Compressed Transfer Encoding

Our oppinion: Runtime Compression pollutes our environment. Unneccesarry consumed CPU-Power. In times of
Intel XEON 6 and 800Gigabit Ethernet *Runtime Based Compression* should be considered as old-fashioned.

Think of a "Web-Pack-Format" which only sends **a single* Metadata+Media-Package at Initial-Client-Request and when App-Updates exist (compressed, not runtime-compressed).

>[!CAUTION]
> Worldwide socket-count *again* reduced by a factor of 1.000.000.

## 1.4. Byte Range Requests

Our primary goal is to drastically speed up Modern-Web-Applications, not being a Streaming-Client. Feature also
ommitted in HTTP/1.2.

>[!TIP]
> Think about using *existing* streaming-protocols designed primarily for streaming purpose!

## 1.5. Pipelined Requests

Pipelined Requests! This key-feature started confusion inside the HTTP-Developer-Community.
Its failure never has been understood correctly.

Instead of correcting the very small design flaw (wrong result ordering) HTTP/2 **copied** already existing (and working) TCP/IP Layer3 features (unneccessarily) into Layer7.

Modern, *generic* OOP design teaches: **NEVER COPY IF YOU CAN AVOID, DONT BE LAZY!**

HTTP/1.2 corrects these design flaws with a single new HTTP-Header: "Request-UUID". Every HTTP-Request puts a Unique Identifier Hash inside the HTTP-Header which will be sent back by the Server in the Response. The Client now is able to allocate the Response to the correct Request even if the Network-Order asynchronously mismatches.

HTTP/1.2 will use stable, existing TCP/IP Layer3 feature to "multiplex" Requests without the need of adding Layer7 logic.

## 1.6. Chunked Encoding

Also completely useless. Seldomly used for sending large data to the client.

>[!TIP]
> Think of using an already existing *File Transfer Protocol* instead.

## 1.7. Text / Binary Based Protocols

In times of Intel XEON 6 Processors a binary protocol is not guaranteed to be faster than a text-based. It makes the following tasks even more complex / error-prone:

- Debugging
- Generic Type Handling
- Parsing

XML improves:

- DTD / Clear Type Definition
- Non Error-Prone Parsing
- Updateable Protocol-Features / Protocol-Versions

Next-Gen-WAP (Web-Application-Protocol) XML Specs RFP see: [WAP-AS-XML-SPECS.md](WAP-AS-XML-SPECS.md).

# 2. Advanced Feature Matrix

| Feature                             | HTTP/1.1                   | HTTP/1.2                                   | HTTP/2                     |
|-------------------------------------|----------------------------|--------------------------------------------|----------------------------|
| CORS                                | :white_check_mark:         | :x: (see 2.1)                              | :white_check_mark:         |
| HTTP Method OPTIONS                 | :white_check_mark:         | :x: (see 2.2)                              | :white_check_mark:         |
| HTTP Method HEAD                    | :white_check_mark:         | :x: (see 2.2)                              | :white_check_mark:         |
| HTTP Method TRACE                   | :white_check_mark:         | :x: (see 2.2)                              | :white_check_mark:         |
| HTTP Method PUT                     | :white_check_mark:         | :x: (see 2.2)                              | :white_check_mark:         |
| HTTP Method DELETE                  | :white_check_mark:         | :x: (see 2.2)                              | :white_check_mark:         |
| Streaming Characteristics           | :white_check_mark:         | :x: (see 2.3)                              | :white_check_mark:         |
| WebSockets                          | :white_check_mark:         | :white_check_mark: (see 2.4)               | :white_check_mark:         |
| Global Up/Downloads                 | :white_check_mark:         | :white_check_mark: (see 2.5, decapsulated) | :white_check_mark:         |

# 2.1. Cross Site Scripting

A clean Application Environment / Setup (including Kubernetes) makes CORS / Cross-Site obsolete. Our Web-Application Design forbids using Cross-Site.

# 2.2. Document Based Methods

These are ancient features. No one needs anymore. Handle via Web-Service. Documents reside on scalable Storage-Backends these days.

# 2.3. Streaming Characteristics

HTML in times of AOL 38.400 Baud Modem lines was built to inline-display content **while loading** the HTML page.

In times of *Intel XEON 6 6980P*, *PCI Express 5.0*, *Kernel DMA* and *800Gbit Ethernet* computers are able to render > 1000 pages with a loading time < 1 second.

So drop this feature in **HTTP/1.2** or **WAP**, however we will call the new protocol suite.

# 2.4. WebSockets

**Long-Polling** was used before WebSockets Protocol has been invented.

**Long-Polling** also works *better* and *more leightweight* when using one single **Keep-Alived** TCP/IP socket (using Request-UUID).

>[!IMPORTANT]
> But: Think of implementing this on a separated (firewallable) TCP/IP port, maybe WACP (Web-Application-Control-Protocol), idea?

So HTTP/1.2 is able to use **Long-Polling** if really needed.

# 2.5. Up / Downloads

Phew, almost forgotten: **Up / Downloads**. It seems likely that protocol-architects did not cope with x86_64 CPU architecture enough. Both HTTP/1.1 and HTTP/2 (not mentioning HTTP/3) involve a catastrophic approach in design and implementation.

What exactly happens when you send data over an TCP/IP socket? Currently, all stream packets will be processed by Kernel (ring-0) and then passed to User-Space (ring-3). This will be done for each single packet (CPU Context-Switch). Due to a slightly oldfashioned Socket-API / Kernel-Interface all packets must be handled additionally in User-Space by the application? **YES**.

Detailed CPU tasks for a **Standard** Application-Upload:

1. Kernel IP-Packet receive
2. Pass IP-Packet to User-Space (1 CPU Context-Switch)
3. Process the Buffer inside Application (e.g. Python Interpreter) - Generates CPU Cycles
4. Pass the Buffer back to Kernel-Space ("write" to File-Descriptor, 1 CPU Context Switch)

HTTP/2 is even worse, implementing flow-control features which already exist in underlaying TCP/IP, so CPU load even raises and throughput reduces.

>[!TIP]
> This could be done easily (completely) in ring-0 like existing file protocols already do (file-descriptor-reference).
