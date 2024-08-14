# 1. ResultOrder Component

The ResultOrder Component manages the following things "inside" ResultProcessor:

- Request "queuing"
- Result ordering for HTTP/1.1 requests

# 2. RFP

## Private Data Members
```
- "Requests" (unordered map)
  - ClientFD (Key)
    - ReqNr (ordered map)
      (HTTPType, SendFileFD, ASIndex)

- "LastRequest" (unordered map)
  - ClientFD (Key)
    - (RequestNr (Default 1), RequestTime (Default now()))

- "LastRequests" (unordered map)
  - ClientFD (Key)
    - (ThreadRef, ReqNrs (vector))
```

## Methods
```
- checkClientFD(ClientFD)
- append(ClientFD, HTTPType, ReqNr, SendFileFD)
- getNextRequests(ClientFD, HTTPType)
- _calcNextRequests()
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
  - ResultOrder.getNextRequests(ClientFD, HTTP1.1) -> ThreadHandler(getVirtualDomain())
  - ResultOrder.getNextRequests(ClientFD, HTTP1.2) -> ThreadHandler(getVirtualDomain())

- After Processing (ResultOrder.::ThreadHandler::Terminator)
  - On ThreadHandler::Terminator::ClientFD::Termination
    - Update "LastRequest" (unordered map) ClientFD RequestNr, RequestTime
    - Remove all processed ClientFD ReqNrs from "Requests", "LastRequests"

- Timeout check (Loop on Requests ClientFDs)
  - If Timeout (now() - LastRequestTime > DefinedTimeout)
    - Remove ClientFD from Requests Map
    - Remove ClientFD from Processed Map
```

