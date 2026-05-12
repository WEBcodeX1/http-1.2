# 2. Next Level Application File Protocol (NLAFP)

## 2.1. Request / Response Static Files

### 2.1.1. Request (Static HTML File)

```xml
<NLAP>
    <Request>
        <UUID>7ea45c8a-5193-4855-b9e8-77ae1b9d49ed</UUID>
        <Protocol>NLAP</Protocol>
        <Version>0.1</Version>
        <Subtype>NLAFP</Subtype>
        <Header>
            <Host>testapp1.local</Host>
            <URL>/testpath/index.html</URL>
            <UserAgent>Falcon-Browser</UserAgent>
        </Header>
    </Request>
</NLAP>
```

### 2.1.2. Response (Inline-HTML)

```xml
<NLAP>
    <Response>
        <UUID>7ea45c8a-5193-4855-b9e8-77ae1b9d49ed</UUID>
        <Protocol>NLAP</Protocol>
        <Version>0.1</Version>
        <Subtype>NLAFP</Subtype>
        <Header>
            <connection-close>1</connection-close>
            <Mime-Type>text/html</Mime-Type>
            <Encoding>UTF-8</Encoding>
        </Header>
        <Payload>
            <html>
                <body>
                    <h1>FalconAS Server</h1>
                    <p>Test-serving page, very old-school.</p>
                </body>
            </html>
        </Payload>
    </Response>
</NLAP>
```

## 2.2. Request / Response Encoded Files

### 2.2.1. Request (Static PNG Image)

```xml
<NLAP>
    <Request>
        <UUID>9a728a72-34ac-9abc-2245-af65cbde66ff</UUID>
        <Protocol>NLAP</Protocol>
        <Version>0.1</Version>
        <Subtype>NLAFP</Subtype>
        <Header>
            <Host>testapp1.local</Host>
            <URL>/testpath/nice-image.png</URL>
            <UserAgent>Falcon-Browser</UserAgent>
        </Header>
    </Request>
</NLAP>
```

## 2.2.2. Response (Encoded File)

```xml
<NLAP>
    <Response>
        <UUID>9a728a72-34ac-9abc-2245-af65cbde66ff</UUID>
        <Protocol>NLAP</Protocol>
        <Version>0.1</Version>
        <Subtype>NLAFP</Subtype>
        <Header>
            <Mime-Type>image/png</Mime-Type>
            <byte-size>7342</byte-size>
            <Compression>none</Compression>
            <Encoding>binary</Encoding>
        </Header>
        <Payload>
        [BINARY_PAYLOAD]
        </Payload>
    </Response>
</NLAP>
```

## 2.3. Sending Large Files In Parts

Sending large files via `sendfile()` even in high-speed network environments causes congestion
(only 1 file can be sent simultaneous over 1 socket until the data has been arrived).

Single files (e.g. 20kb to 300kb) will not cause performance degradation on modern ethernet
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
<NLAP>
    <Request>
        <UUID>f3477af2-1212-76af-3377-bc7721afbc7a</UUID>
        <Protocol>NLAP</Protocol>
        <Version>0.1</Version>
        <Subtype>NLAFP</Subtype>
        <Header>
            <Host>testapp1.local</Host>
            <URL>/testpath/package.bz2</URL>
            <UserAgent>Falcon-Browser</UserAgent>
        </Header>
    </Request>
</NLAP>
```

## 2.3.2. Partial Response (First Part)

```xml
<NLAP>
    <Response>
        <UUID>f3477af2-1212-76af-3377-bc7721afbc7a</UUID>
        <Protocol>NLAP</Protocol>
        <Version>0.1</Version>
        <Subtype>NLAFP</Subtype>
        <Header>
            <Mime-Type>application/bzip2</Mime-Type>
            <byte-size-full>3432132</byte-size-full>
            <byte-size-part>100000</byte-size-part>
            <Compression>none</Compression>
            <Encoding>binary</Encoding>
            <File-UUID>f3477af2-1212-76af-3377-bc7721afbc7a</File-UUID>
            <File-Part-Sum>35</File-Part-Sum>
            <File-Part>1</File-Part>
        </Header>
        <Payload>
        [BINARY_PAYLOAD]
        </Payload>
    </Response>
</NLAP>
```

## 2.4. Signing Requests / Responses

To prevent rquest / response manipulation, signing requests and responses is possible.

- Signing client requests with a Private Key
- Signing server responses with the server Private Key

Detailed description can be found in NLAPP (Next Level Application Proxy Protocol) documentation,
