# 1. ResultScheduler Component

The ResultScheduler Component manages the following things "inside" ResultProcessor:

- Request "queuing"
- Result ordering for HTTP/1.1 requests

# 2. RFP

## Private Data Members
```
- "Requests" (unordered map)
  - ClientFD (Key)
    - ReqNr (map)
      { HTTPType, SendFileFD, ASIndex }

- "LastRequest" (unordered map)
  - ClientFD (Key), RequestNr

- "LastRequests" (unordered map)
  - ClientFD (Key)
    - { ThreadRef, ReqNrs (vector) }
```

## Methods
```
- void append(ClientFD, HTTPType, ReqNr, SendFileFD)
- void processClients()
- ResultPropsList_t getNextRequests(ClientFD, HTTPType)
```

## ResultProcessor Handling

```
- On StaticFS Data
  - ResultOrder.append(ClientFD, HTTPType, ReqNr, SendFileFD)
- On POST AS Data
  - ResultOrder.append(ClientFD, HTTPType, ReqNr, ASIndex)

- Process (loop on "Requests" ClientFDs)
  - If ClientFD not in "LastRequest"
    - Insert into "LastRequest" with Default RequestNr 1 and RequestTime now()
  - ResultOrder.getNextRequests(ClientFD, HTTP1.2) -> ThreadHandler(getVirtualDomain())
  - ResultOrder.getNextRequests(ClientFD, HTTP1.1) -> ThreadHandler(getVirtualDomain())
  
  # Process in serial order. First: HTTP/1.2, afterwards HTTP/1.1. If no HTTP/1.2 request: process HTTP/1.1.

- On ThreadHandler::Terminator::ClientFD::Termination (Callback pointer)
  - Update "LastRequest" (unordered map) ClientFD RequestNr, RequestTime
  - Remove all processed ClientFD ReqNrs from "Requests", "LastRequests"
```

