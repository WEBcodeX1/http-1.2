# Component::ASProcessHandler

## OnServerStart

AS Index = 0
- Loop on Config::VirtualDomains
  - Loop on VirtualDomain::ASCount
    - Setup AS[Index] Process / SHM Reference
    - Index++

## Shared Memory Layout

```bash
-- AS 1 Metadata ------------------------------------------------

0x0000              atomic_uint8_t      ReadFinished    0
0x0001              atomic_uint8_t      WriteReady      0
0x0002              atomic_uint8_t      Busy            0

0x0003              uint64_t            ClientFD        Nullptr
0x00ob              uint8_t             HTTPVersion     1
0x000c              uint16_t            ReqNr           1
0x000e              uint32_t            PayloadLength   Nullptr

-- AS 2 Metadata ------------------------------------------------

0x0012              atomic_uint8_t      ReadFinished    0
0x0013              atomic_uint8_t      WriteReady      0
0x0014              atomic_uint8_t      Busy            0

0x0015              uint64_t            ClientFD        Nullptr
0x001d              uint8_t             HTTPVersion     1
0x001e              uint16_t            ReqNr           1
0x0020              uint32_t            PayloadLength   Nullptr

-- AS 1 Payload -------------------------------------------------

0x0024              char[PayloadLenA1]  Payload         Nullptr

-- AS 2 Payload -------------------------------------------------

0x0024+max_pl_bytes char[PayloadLenA2]  Payload         Nullptr
```

# Component::ResultProcessor

Logical segmentation:

- Check / Read GET requests
- Check / Read POST results
- Thread Processor (1 Thread, 1 FD requests, terminate)

## Main Loop

```python
loop:

  workdone = 0

  - if get_read_data == 1
      append GET static FS requests into ResultProcessor::ResultOrder
      workdone = 1
  - check all_as_procs_non_busy_read_finished || all_as_procs_busy
      write 0 get_read_data
      workdone = 1
  - check can_get_post_results
      if can_get_post_result_1
        append POST result into ResultProcessor::ResultOrder
        workdone = 1
      if can_get_post_result_X
        append POST result into ResultProcessor::ResultOrder
        workdone = 1
  - if fds_to_process
      ProcessThreads()
      workdone = 1

  - if workdone == 0:
    sleep
```

## Process Threads function

- Check: If ClientFD Thread busy
  - addWaitQueue (GET and POST)
  - else:
    startupThread()

# Component::ResultProcessor::ThreadProcessor

- CheckThreadsJoinable()
- startupThread(
   #params following
  )

# Component::ResultProcessor::ResultOrder

ClientFD
  ReqNr (PK, Order)
  Type
  SendFileFD
  ASIndex


WaitForNextReqNr  1

CheckNext     while not next: ReqNr+1


# Server::MainLoop modification

- AddSHMQueue ALL Get Requests
- AddSHMQueue count(nonbusyAS)

Rest: add WaitQueue

