# Performance Tests

Performance Tests will record statistical data about *Static File* and *Application Server* components.

# Global Settings

Tests include the following subsections:

- Static File Performance
- Performance under Idle Keep-Alive Connections 
- Application Server Dynamic Data
- Proxy Server Performance
- Comparison to Apache2 and nginX external Web-Servers

All Tests will be primarily done on FalconAS Server using HTTP/1.2 and HTTP/1.1 protocol.

All Tests will be repeated externally on different web-servers using protocols:

- HTTP/1.1
- HTTP/2

# External Products

All Tests will be repeated on the following web-server products:

- Apache2 Threaded Model
- Apache2 Event Driven Model
- nginX

# Detailed  Workflow

1. Insert (TestID, TestIDRepeat) INTO "Test" table
2. Insert Server Test Parameter into "TestParameter" table
3. Write Server XML configuration
4. Start Server
5. Run / Loop Tests, write stat results into C++ structs
6. Insert Test Results into "TestMeasurement" table

# Test Specs (Google Sheet)

https://docs.google.com/spreadsheets/d/1Da3KsIfTPGJOpeS1Ns2pHvEU_RpdWwx8kU5Ug-b_ZAE/edit?usp=drive_link
