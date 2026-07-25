# 1. Next Level Application Metadata Protocol (NLAMP)

## 1.1. Request / Response Backend Services

### 1.1.1. Application Request (JSON)

```xml
<nlap>
    <Request>
        <UUID>9b327afe-27ae-2367-aef2-e42445e5b23a</UUID>
        <Protocol>NLAP</Protocol>
        <Version>0.1</Version>
        <Subtype>NLAMP</Subtype>
        <Header>
            <Host>testapp2.local</Host>
            <URL>/python/service1</URL>
            <User-Agent>Falcon-Python-Client</User-Agent>
        </Header>
        <Payload>
            {
                "param1": "string1",
                "param2": "string2",
                "param3": 100
            }
        </Payload>
    </Request>
</nlap>
```

### 1.1.2. Data Response (JSON)

```xml
<nlap>
    <Response>
        <UUID>9b327afe-27ae-2367-aef2-e42445e5b23a</UUID>
        <Protocol>NLAP</Protocol>
        <Version>0.1</Version>
        <Subtype>NLAMP</Subtype>
        <Header>
            <Mime-Type>application/json</Mime-Type>
            <Encoding>UTF-8</Encoding>
        </Header>
        <Payload>
            { "Result": 100 }
        </Payload>
        <Status>
            <Code>0</Code>
        </Status>
    </Response>
</nlap>
```

### 1.1.3. Failure Response

```xml
<nlap>
    <Response>
        <UUID>9b327afe-27ae-2367-aef2-e42445e5b23a</UUID>
        <Protocol>NLAP</Protocol>
        <Version>0.1</Version>
        <Subtype>NLAMP</Subtype>
        <Header>
            <Mime-Type>application/xml</Mime-Type>
            <Encoding>UTF-8</Encoding>
        </Header>
        <Status>
            <Code>10</Code>
            <Description>Application Exception</Description>
            <Exception>NameError: name 'test' is not defined</Exception>
        </Status>
    </Response>
</nlap>
```
