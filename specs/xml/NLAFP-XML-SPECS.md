# 2. Next Level Application File Protocol (NLAFP)

## 2.1. Request / Response Static Files

### 2.1.1. Request (Static HTML File)

```xml
<request>
    <UUID>7ea45c8a-5193-4855-b9e8-77ae1b9d49ed</UUID>
    <protocol>NLAP</protocol>
    <version>0.1</version>
    <subtype>NLAFP</subtype>
    <header>
        <host>testapp1.local</host>
        <URL>/testpath/index.html</URL>
        <UserAgent>Falcon-Browser</UserAgent>
    </header>
</request>
```

### 2.1.2. Response (Inline-HTML)

```xml
<response>
    <UUID>7ea45c8a-5193-4855-b9e8-77ae1b9d49ed</UUID>
    <protocol>NLAP</protocol>
    <version>0.1</version>
    <subtype>NLAFP</subtype>
    <header>
        <connection-close>1</connection-close>
        <mime-type>text/html</mime-type>
        <encoding>UTF-8</encoding>
    </header>
    <payload>
        <html>
            <body>
                <h1>FalconAS Server</h1>
                <p>Test-serving page, very old-school.</p>
            </body>
        </html>
    </payload>
</response>
```

## 2.2. Request / Response Encoded Files

### 2.2.1. Request (Static PNG Image)

```xml
<request>
    <UUID>9a728a72-34ac-9abc-2245-af65cbde66ff</UUID>
    <protocol>NLAP</protocol>
    <version>0.1</version>
    <subtype>NLAFP</subtype>
    <header>
        <host>testapp1.local</host>
        <URL>/testpath/nice-image.png</URL>
        <UserAgent>Falcon-Browser</UserAgent>
    </header>
</request>
```

## 2.2.2. Response (Encoded File)

```xml
<response>
    <UUID>9a728a72-34ac-9abc-2245-af65cbde66ff</UUID>
    <protocol>NLAP</protocol>
    <version>0.1</version>
    <subtype>NLAFP</subtype>
    <header>
        <mime-type>image/png</mime-type>
        <byte-size>7342</byte-size>
        <compression>none</compression>
        <encoding>binary</encoding>
    </header>
    <payload>
    [BINARY_PAYLOAD]
    </payload>
</response>
```

## 2.3. Sending Large Files In Parts

Sending large files via `sendfile()` even in high-speed network environments causes congestion
(only 1 file can be sent simultaneous over 1 socket until the data has been arrived).

Single files (e.g. 20kb to 300kb) will not cause performance degregation on modern ethernet
infrastructure including internet routing. Packet retransmissions / dynamic routing issues should
be "healed" in milliseconds.

If this will not be the case, there is some severe connection problem which probably is not
healable even by QUIC HTTP/3 protocols.

Large files without sending in parts will still be a latency problem, if sending two files 20mb
size each will cause application latency (waiting until the files have arrived).

The solution is: do not use `sendfile()` transmission for large files / and or flag files in
the configuration to chose between sending via `sendfile()` or NLAFP partial transfer feature
/ `write()` kernel syscall.

### 2.3.1. Request (Static BZIP Image)

```xml
<request>
    <UUID>f3477af2-1212-76af-3377-bc7721afbc7a</UUID>
    <protocol>NLAP</protocol>
    <version>0.1</version>
    <subtype>NLAFP</subtype>
    <header>
        <host>testapp1.local</host>
        <URL>/testpath/package.bz2</URL>
        <UserAgent>Falcon-Browser</UserAgent>
    </header>
</request>
```

## 2.3.2. Partial Response (First Part)

```xml
<response>
    <UUID>f3477af2-1212-76af-3377-bc7721afbc7a</UUID>
    <protocol>NLAP</protocol>
    <version>0.1</version>
    <subtype>NLAFP</subtype>
    <header>
        <mime-type>application/bzip2</mime-type>
        <byte-size-full>3432132</byte-size-full>
        <byte-size-part>100000</byte-size-part>
        <compression>none</compression>
        <encoding>binary</encoding>
        <File-UUID>f3477af2-1212-76af-3377-bc7721afbc7a</File-UUID>
        <File-Part-Sum>35</File-Part-Sum>
        <File-Part>1</File-Part>
    </header>
    <payload>
    [BINARY_PAYLOAD]
    </payload>
</response>
```

## 2.4. Signing Requests / Responses

To prevent rquest / response manipulation, signing requests and responses is possible.

- Signing client requests with a Private Key
- Signing server responses with the server Private Key

Detailed description can be found in NLAPP (Next Level Application Proxy Protocol) documentation,
