# 5. Next Level Application Protocol Secure Extension (NLAPS)

## 5.1. Request / Responses

### 5.1.1. Encrypted / Signed Request

```xml
<NLAP>
    <Request>
        <UUID>a2327a55-33ae-2557-aef2-e42445e5b23a</UUID>
        <Protocol>NLAP</Protocol>
        <Version>0.1</Version>
        <Subtype>NLAMP</Subtype>
        <Header>
            <Host>testapp2.local</Host>
            <user>user1@domain.com</user>
            <URL>/python/service1</URL>
            <User-Agent>Falcon-Python-Client</User-Agent>
        </Header>
        <Security>
            <Encryption>1</Encryption>
            <Signature>
                #BASE64-ENCODED-SIGNATURE
            </Signature>
        </Security>
        <Payload>
            #BASE64-ENCODED-ENCRYPTED-PAYLOAD
        </Payload>
    </Request>
</NLAP>
```

### 5.1.2. Encrypted / Signed Response

```xml
<NLAP>
    <Response>
        <UUID>a2327a55-33ae-2557-aef2-e42445e5b23a</UUID>
        <Protocol>NLAP</Protocol>
        <Version>0.1</Version>
        <Subtype>NLAMP</Subtype>
        <Header>
            <Mime-Type>application/json</Mime-Type>
            <Encoding>UTF-8</Encoding>
        </Header>
        <Security>
            <Encryption>1</Encryption>
            <Signature>
                #BASE64-ENCODED-SIGNATURE
            </Signature>
        </Security>
        <Payload>
            #BASE64-ENCODED-ENCRYPTED-PAYLOAD
        </Payload>
        <Status>
            <Code>0</Code>
        </Status>
    </Response>
</NLAP>
```
