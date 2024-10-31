# 1. Feature Matrix (Basic)

The following table shows features compared to the different HTTP Versions.

| Feature                             | HTTP/1.1               | HTTP/1.2               | HTTP/2                 |
|-------------------------------------|------------------------|------------------------|------------------------|
| Keep-Alive Connections              | x                      | x (permanent, see 1.1) | x                      |
| Parametrized Keep-Alive (Header)    | x                      | - (see 1.2)            | x                      |
| Transfer Encoding (Compressed)      | x                      | - (see 1.3)            | x                      |
| Byte Range Requests                 | x                      | - (see 1.4)            | x                      |
| Pipelined Requests                  | x (broken)             | x (working, see 1.5)   | - (Layer7 multiplexed) |
| Chunked Encoding                    | x                      | - (see 1.6)            | x                      |
| Caching Mechanism / Header          | x                      | x (Static Content)     | x                      |
| Caching 304 not modified            | x                      | x (Static Content)     | x                      |
| Cookies                             | x                      | x                      | x                      |
| Text Based Protocol                 | x                      | x                      | -                      |
| Binary Based Protocol               | -                      | - (see 1.7)            | x                      |

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
> Think about *existing* streaming-protocols designed primarily for streaming purpose!

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
> Think of using a *File Transfer Protocol* instead.

## 1.7. Text / Binary Based Protocols

In times of Intel XEON 6 Processors a binary protocol is not guaranteed to be faster than a text-based. It makes the following tasks even more complex / error-prone:

- Debugging
- Generic Type Handling
- Parsing

XML improves:

- DTD / Clear Type Definition
- Non Error-Prone Parsing
- Updateable Protocol-Features / Protocol-Versions

Next-Gen-WAP (Web-Application-Protocol) should use XML format like this:

### 1.7.1. Request (StaticFile)

```xml
<header>
    <version>WAP/0.1</version>
    <variant>Request</variant>
    <host>testapp1.local</host>
    <URL>/testpath/index.html</URL>
    <UserAgent>Falcon-Browser</UserAgent>
    <Request-UUID>7ea45c8a-5193-4855-b9e8-77ae1b9d49ed</Request-UUID>
</header>
```

### 1.7.2. Response (HTML)

```xml
<header>
    <version>WAP/0.1</version>
    <variant>Response</variant>
    <connection-close>1</connection-close>
    <mime-type>text/html</mime-type>
    <encoding>UTF-8</encoding>
    <Request-UUID>7ea45c8a-5193-4855-b9e8-77ae1b9d49ed</Request-UUID>
</header>
<payload>
    <html>
        <body>
            <h1>Server Status</h1>
            <p>Busy. Good bye...</p>
        </body>
    </html>
</payload>
```

### 1.7.3. Request (Application)

```xml
<header>
    <version>WAP/0.1</version>
    <variant>Request</variant>
    <host>testapp1.local</host>
    <URL>/python/svc1</URL>
    <UserAgent>Falcon-Application-Controller</UserAgent>
    <mime-type>XML-native</mime-type>
    <Request-UUID>70936869-3b17-4bcd-89fb-72adf57acc27</Request-UUID>
    <payload>
        <param1>Value1</param1>
        <param2>Value2</param2>
        <param3>Value3</param3>
    </payload>
</header>
```

### 1.7.2. Response (JSON)

```xml
<header>
    <version>WAP/0.1</version>
    <variant>Response</variant>
    <mime-type>application/json</mime-type>
    <encoding>UTF-8</encoding>
    <Request-UUID>70936869-3b17-4bcd-89fb-72adf57acc27</Request-UUID>
</header>
<payload>
  { "Result": 100 }
</payload>
```

# 2. Advanced Feature Matrix

| Feature                             | HTTP/1.1               | HTTP/1.2               | HTTP/2                 |
|-------------------------------------|------------------------|------------------------|------------------------|
| CORS                                | x                      | - (see 2.1)            | x                      |
| HTTP Method OPTIONS                 | x                      | - (see 2.2)            | x                      |
| HTTP Method HEAD                    | x                      | - (see 2.2)            | x                      |
| HTTP Method TRACE                   | x                      | - (see 2.2)            | x                      |
| HTTP Method PUT                     | x                      | - (see 2.2)            | x                      |
| HTTP Method DELETE                  | x                      | - (see 2.2)            | x                      |
| Streaming Characteristics           | x                      | - (see 2.3)            | x                      |

# 2.1. Cross Site Scripting

A clean Application Environment / Setup (including Kubernetes) makes CORS / Cross-Site obsolete. Our Web-Application Design forbids using Cross-Site.

# 2.2. Document Based Methods

These are ancient features. No one needs anymore. Handle via Web-Service. Documents reside on scalable Storage-Backends these days.

# 2.3. Streaming Characteristics

HTML in times of AOL 38.400 Baud Modem lines was built to inline-display content **while loading** the HTML page.

In times of *Intel XEON 6 6980P*, *PCI Express 5.0*, *Kernel DMA* and *800Gbit Ethernet* computers are able to render > 1000 pages with a loading time < 1 second.

So drop this feature in **HTTP/1.2** or **WAP**, however we will call the new protocol suite.

# 3. Bottlenecks
#TODO: write.

## 3.1. Ring-0 / Context Switching
#TODO: write.

## 3.2. TLS
#TODO: write.

## 3.3. Global Security
#TODO: write.
