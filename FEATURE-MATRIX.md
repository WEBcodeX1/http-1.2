# 1. Feature Matrix (Basic)

The following table shows features compared to the different HTTP Versions.
HTTP/3 has been ommited because of completely different protocol design / no backward compatibility.

| Feature                             | HTTP/1.1               | HTTP/1.2               | HTTP/2                 |
|-------------------------------------|------------------------|------------------------|------------------------|
| Keep-Alive Connections              | x                      | x (permanent, see #1)  | x                      |
| Parametrized Keep-Alive (Header)    | x                      | - (see #2)             | x                      |
| Transfer Encoding (Compressed)      | x                      | - (see #3)             | x                      |
| Byte Range Requests                 | x                      | - (see #4)             | x                      |
| Pipelined Requests                  | x (broken)             | x (working, see #5)    | - (Layer7 multiplexed) |
| Chunked Encoding                    | x                      | - (see #6)             | x                      |
| Caching Mechanism / Header          | x                      | x (Static Content)     | x                      |
| Caching 304 not modified            | x                      | x (Static Content)     | x                      |
| Cookies                             | x                      | x                      | x                      |
| Text Based Protocol                 | x                      | x                      | -                      |
| Binary Based Protocol               | -                      | - (see #7)             | x                      |

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

Next-Gen-WACP (Web-Application-Control-Protocol) should use XML format like this:

### 1.7.1. Request (StaticFile)

```xml
<header>
    <version>WACP/0.1</version>
    <variant>Request</variant>
    <host>testapp1.local</host>
    <URL>/testpath/index.html</URL>
    <UserAgent>Falcon-Browser</UserAgent>
</header>
```

### 1.7.2. Response (HTML)

```xml
<header>
    <version>WACP/0.1</version>
    <variant>Response</variant>
    <connection-close>1</connection-close>
    <mime-type>text/html</mime-type>
    <encoding>UTF-8</encoding>
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
    <version>WACP/0.1</version>
    <variant>Request</variant>
    <host>testapp1.local</host>
    <URL>/python/svc1</URL>
    <UserAgent>Falcon-Application-Controller</UserAgent>
    <mime-type>XML-native</mime-type>
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
    <version>WACP/0.1</version>
    <variant>Response</variant>
    <mime-type>application/json</mime-type>
    <encoding>UTF-8</encoding>
</header>
<payload>
  { "Result": 100 }
</payload>
```

# 2. Advanced Feature Matrix



# 3. Bottlenecks

## 3.1. Ring-0 / Context Switching

## 3.2. TLS

