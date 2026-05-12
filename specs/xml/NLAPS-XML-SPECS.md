# 4. Next Level Application Protocol Secure Extension (NLAPS)

## 4.1. Request / Responses

### 4.1.1. Encrypted / Signed Request

```xml
<request>
    <UUID>a2327a55-33ae-2557-aef2-e42445e5b23a</UUID>
    <protocol>NLAP</protocol>
    <version>0.1</version>
    <subtype>NLAMP</subtype>
    <header>
        <host>testapp2.local</host>
        <user>user1@domain.com</user>
        <URL>/python/service1</URL>
        <UserAgent>Falcon-Python-Client</UserAgent>
    </header>
    <security>
        <encrpytion>1</encrpytion>
        <signature>
            #BASE64-ENCODED-SIGNATURE
        </signature>
    </security>
    <payload>
        #BASE64-ENCODED-ENCRYPTED-PAYLOAD
    </payload>
</request>
```

### 4.1.2. Encrypted / Signed Response

```xml
<response>
    <UUID>a2327a55-33ae-2557-aef2-e42445e5b23a</UUID>
    <protocol>NLAP</protocol>
    <version>0.1</version>
    <subtype>NLAMP</subtype>
    <header>
        <mime-type>application/json</mime-type>
        <encoding>UTF-8</encoding>
    </header>
    <security>
        <encrpytion>1</encrpytion>
        <signature>
            #BASE64-ENCODED-SIGNATURE
        </signature>
    </security>
    <payload>
        #BASE64-ENCODED-ENCRYPTED-PAYLOAD
    </payload>
    <status>
        <code>0</code>
    </status>
</response>
```
