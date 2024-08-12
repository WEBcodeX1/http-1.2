# Client Handler

The ClientHandler Component handles Client Sockets / Epoll Setup / Data Buffering.

## Program Logic

### Add Client

Called from Main::Server on new Client Socket.

- Insert ClientObject with ClientFD as Key into internal C++ Map
- Add ClientFD to Kernel Epoll

### Process Clients

Get all Epoll FDs with waiting data.

> If 5000 current Clients are connected and 2000 have waiting buffered data, we get an
Array of these 2000 Filedescriptor integers.

Process all Filedescriptors calling readClientData().

### Read Client Data

Process all Filedescriptors with waiting data.

- Close connection when connection-close (0 Bytes received) on Socket Level
- If Filedesriptor in Client Map found, append received data to Client Objects Buffer
- Parse Request Basic Data, if non-fragmented add to SHM buffer
- If Clients with data exist, release SHM StaticFS Lock to process
