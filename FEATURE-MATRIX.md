# Feature Matrix

The following table shows features compared to the different HTTP Versions.
HTTP/3 has been ommited because of completely different protocol design / no backward compatibility.

| Feature                             | HTTP/1.1               | HTTP/1.2               | HTTP/2                 |
|-------------------------------------|------------------------|------------------------|------------------------|
| Keep-Alive Connections              | x                      | x (permanent, see #1)  | x                      |
| Parametrized Keep-Alive (Header)    | x                      | - (see #2)             | x                      |
| Transfer Encoding (Compressed)      | x                      | - (see #3)             | x                      |
| Byte Range Requests                 | x                      | - (see #4)             | x                      |
| Pipelined Requests                  | x (broken)             | x (working, see #5)    | - (multiplexed)        |
| Chunked Encoding                    | x                      | - (see #6)             | x                      |
| Caching Mechanism / Header          | x                      | x (Static Content)     | x                      |
| Caching 304 not modified            | x                      | x (Static Content)     | x                      |
| Cookies                             | x                      | x                      | x                      |
| Text Based Protocol                 | x                      | x                      | -                      |
| Binary Based Protocol               | -                      | - (see #7)             | x                      |

See []() for a detailed explanation / analysis why HTTP/2 and HTTP/3 are not suitable for future-proof
Web-Applications.

# Permanent Keep-Alive Connection (#1)

HTTP/1.2 uses *permanent* Keep-Alive. This means a single Client always connects through 1 single socket to a
Server Domain / Virtual Host.

# Parametrized Keep-Alive (#2)

Due to a permanent Keep-Alive, we also do not need Parametrized Keep-Alive settings which will drastically reduce
Protocol-Logic.

# Compressed Transfer Encoding (#3)

Our oppinion: Runtime Compression pollutes our environment. Unneccesarry consumed CPU-Power. And in times of
Intel XEON 6 and 800Gigabit Ethernet Runtime Based Compression should be considered as old-fashioned.

Think of a "Web-Pack-Format" which only sends "ONE" Metadata+Media-Package at Initial-Request and App-Updates
(compressed, not runtime-compressed).

# Byte Range Requests (#4)

Our primary goal is to drastically speed up Modern-Web-Applications, not being a Streaming-Client. Feature also
ommitted in HTTP/1.2.

# Pipelined Requests (#5)

Pipelined Requests! This HTTP/1.1 feature is responsible for the current HTTP/2 and HTTP/3 desaster.

The non-correct-working response-ordering made HTTP/1.1 fail for high-speed asynchronous Web-Applications.

We correct this by adding the UUID Header ... This must be implemented on Client-Side ...
#TODO: finish

# Chunked Encoding (#6)

Also completely useless .
#TODO: finish

# Text / Binary Based Protocols (#7)

Switching to Protocol-Binary-Format in times of Intel XEON 6 in our oppinion is the wrong decision.

We even tend to specify a HTTP-Request in XML format (see []()).

# Bottlenecks

## Ring-0 / Context Switching

## TLS

