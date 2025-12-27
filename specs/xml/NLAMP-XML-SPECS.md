# 1. Next Level Application Metadata Protocol (NLAMP)

## 1.1. Request / Response Backend Services

### 1.1.1. Application Request (JSON)

```xml
<request>
    <UUID>9b327afe-27ae-2367-aef2-e42445e5b23a</UUID>
    <protocol>NLAP</protocol>
    <version>0.1</version>
    <subtype>NLAMP</subtype>
    <header>
        <host>testapp2.local</host>
        <URL>/python/service1</URL>
        <UserAgent>Falcon-Python-Client</UserAgent>
    </header>
    <payload>
        {
            "param1": "string1",
            "param2": "string2",
            "param3": 100
        }
    </payload>
</request>
```

### 1.1.2. Data Response (JSON)

```xml
<response>
    <UUID>9b327afe-27ae-2367-aef2-e42445e5b23a</UUID>
    <protocol>NLAP</protocol>
    <version>0.1</version>
    <subtype>NLAMP</subtype>
    <header>
        <mime-type>application/json</mime-type>
        <encoding>UTF-8</encoding>
    </header>
    <payload>
        { "Result": 100 }
    </payload>
    <status>
        <code>0</code>
    </status>
</response>
```

### 1.1.3. Failure Response

```xml
<response>
    <UUID>9b327afe-27ae-2367-aef2-e42445e5b23a</UUID>
    <protocol>NLAP</protocol>
    <version>0.1</version>
    <subtype>NLAMP</subtype>
    <header>
        <mime-type>application/xml</mime-type>
        <encoding>UTF-8</encoding>
    </header>
    <status>
        <code>10</code>
        <description>Application Exception</description>
        <exception>NameError: name 'test' is not defined</exception>
    </status>
</response>
```
