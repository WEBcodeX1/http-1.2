# Database Definition for collecting Performance Tests Data

Details following.

## Tables

### Table "Test"

| column                   | type                                | default        | null     |
|--------------------------|-------------------------------------|----------------|----------|
| ID                       | bigserial                           |                |          |
| TestID                   | string                              |                | NOT NULL |
| TestIDRepeat             | serial                              | 0              |          |
| StartDate                | datetime 'YYYY-mm-dd hh:mm'         | now()          | NOT NULL |
| EndDate                  | datetime 'YYYY-mm-dd hh:mm'         |                |          |

PrimaryKey:	ID
UniqueKeys: TestID, TestIDRepeat

### Table "TestMeasurement"

| column                   | type                                | default        | null     |
|--------------------------|-------------------------------------|----------------|----------|
| ID                       | bigserial                           |                |          |
| TestID                   | bigint (FK: "Test"."ID")            |                |          |
| TestName                 | string                              |                | NOT NULL |
| TestNr                   | int                                 |                | NOT NULL |
| HTTPProtocol             | string enum (1.1, 1.2, 2.0)         |                | NOT NULL |
| ServerType               | string (FalconAS, Apache2, NginX)   |                | NOT NULL |
| ReqType                  | string enum (GET, POST)             |                | NOT NULL |
| GetModified              | bool (true, false)                  | true           |          |
| ClientNr                 | int                                 |                | NOT NULL |
| ThreadNr                 | int                                 |                |          |
| ReqData                  | string                              |                | NOT NULL |
| ReqSendGetTimeNano       | int 64bit                           |                | NOT NULL |
| ReqGetDataTimeNano       | int 64bit                           |                | NOT NULL |
| ReqServerProcessTimeNano | int 64bit                           |                |          |
| ReqErrorCode             | string                              |                | NOT NULL |

PrimaryKey:	ID
UniqueKeys: TestID, TestName, TestNr

### Table "TestParameter"

| column                   | type                                | default        | null     |
|--------------------------|-------------------------------------|----------------|----------|
| ID                       | bigserial                           |                |          |
| TestID                   | bigint (FK: "Test"."ID")            |                |          |
| ParameterID              | string                              |                | NOT NULL |
| ParameterValue           | string                              |                | NOT NULL |

PrimaryKey:	ID

