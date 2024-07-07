# Performance Tests

HTTP1.2 tests include WEB/codeX FalconAS *web-server* / *application server* which runs currently on Linux Kernel 6.5.

# Global Settings

Tests include the following subsections:

- Static File Performance
- Performance under Idle Keep-Alive Connections 
- Application Server Dynamic Data
- Proxy Server Performance
- Comparison to Apache2 and nginX external Web-Servers

All Tests will be primarily done on FalconAS Server with HTTP1.2 protocol.

All Tests will be repeated internally using protocols:

- HTTP1.1

All Tests will be repeated externally on different web-servers using protocols:

- HTTP1.1
- HTTP2

All Tests will be repeated on external web.servers:

- Apache2 Threaded Model
- Apache2 Event Driven Model
- nginX

Also tests will be repeated with Test-Parameters defined in "./specs/HTTP_1.2 Test-Specs.xlsx" sheet.

# Detailed  Workflow

1. Insert (TestID, TestIDRepeat) INTO "Test" table
2. Insert Server Test Parameter into "TestParameter" table
3. Write Server XML configuration
4. Start Server
5. Run / Loop Tests, write stat results into C++ structs
6. Insert Test Results into "TestMeasurement" table

