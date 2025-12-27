# 1. XML Specifications (WACP-AS Protocol)

## 1.1. Examples

### 1.1.1. Request (StaticFile)

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

### 1.1.2. Response (Inline-HTML)

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

### 1.1.3. Request (Application)

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
        <param id="param1" type="string">Value1</param>
        <param id="param2" type="string">Value2</param>
        <param id="param3" type="uint16_t>340</param>
    </payload>
</header>
```

### 1.1.4. Response (JSON)

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

### 1.1.5. Response (Status)

```xml
<header>
    <version>WAP/0.1</version>
    <variant>Response</variant>
    <mime-type>application/xml-inline</mime-type>
    <encoding>UTF-8</encoding>
    <Request-UUID>d8f76180-411c-4128-bd06-c0f8edcc2597</Request-UUID>
</header>
<payload>
    <param id="res-string" type="string">Error in Big-Data processing</param>
    <param id="res-id" type="uint16_t">20</param>
    <param id="node-id" type="string">kube-us-nw.valkey-01.domain.io</param>
</payload>
<status>
    <error>
        <msg>Backend Error occured.</msg>
        <id>500</id>
        <inform>admin@domain.com</inform>
    </error>
</status>
```
