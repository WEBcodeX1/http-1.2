#include "ClientHandler.hpp"

using namespace std;

ClientHandler::ClientHandler() :
    ProcessedClients(0),
    BufferMemory(CLIENTS_MAX, BUFFER_SIZE)
{
    DBG(120, "Constructor");

    //- init clients map
    Clients.reserve(CLIENTS_MAX);

    //- setup epoll
    EpollFD = epoll_create1(0);
}

ClientHandler::~ClientHandler()
{
    DBG(120, "Destructor");
}

void ClientHandler::setSharedMemPointer(void* SharedMemPointer) {
    _SharedMemBase = SharedMemPointer;
}

void ClientHandler::addClient(const uint16_t ClientFD)
{
    DBG(70, "Add client with FD:" << ClientFD);

    //- set client connection non blocking
    Socket::makeNonblocking(ClientFD);

    ClientRef_t ClientObj(new HTTPParser(ClientFD));

    Clients.insert(
        ClientMapPair_t(ClientFD, ClientObj)
    );

    //- add fd to epoll
    EpollEvent.events = EPOLLIN | EPOLLET;
    EpollEvent.data.fd = ClientFD;

    epoll_ctl(EpollFD, EPOLL_CTL_ADD, ClientFD, &EpollEvent);
}

void ClientHandler::processClients()
{
    //- reset processed clients
    ProcessedClients = 0;

    //- get epoll ready filedescriptors
    int FDCount = epoll_wait(
        EpollFD,
        EpollEvents,
        EPOLL_FD_COUNT_MAX,
        0
    );

    //- on error
    if (FDCount == -1) {
        ERR("Epoll Error:" << errno);
        ProcessedClients = 0;
        return;
    }

    //- if filedesctiptors to process
    if (FDCount > 0) {
        DBG(190, "Epoll ready FD count:" << FDCount);
        //- read client data
        readClientData(FDCount);
    }
}

void ClientHandler::readClientData(const uint16_t FDCount)
{
    DBG(70, "Read client data. Filedescriptor count:" << FDCount);

    //- reset base shm pointer
    void* CientSharedMemData = static_cast<char*>(_SharedMemBase) + sizeof(atomic_int64_t) + sizeof(uint16_t);

    DBG(100, "Parent Server CientSharedMemData Address:" << CientSharedMemData);

    //- reset processed clients count
    uint16_t ProcessedClients = 0;

    //- process all filedescriptors with data (or close)
    for (uint16_t i=0; i<FDCount; ++i) {

        //char* Buffer[BUFFER_SIZE];
        uint16_t ReadFD = EpollEvents[i].data.fd;

        // read data into buffer
        char* Buffer = BufferMemory.getNextMemPointer();
        uint16_t RcvBytes = read(ReadFD, Buffer, BUFFER_SIZE);

        //- client close connection
        if (RcvBytes == 0) {
            DBG(100, "Client closed connection. Removing from processing map, closing FD:" << ReadFD);
            if (Clients.contains(ReadFD)) {
                Clients.erase(ReadFD);
            }
            close(ReadFD);
        }
        else {
            //- if filedescriptor exists in map, append buffer data
            if (Clients.contains(ReadFD)) {
                ClientRef_t ClientRef = Clients.at(ReadFD);
                ClientRef->appendBuffer(Buffer, RcvBytes);
                CientSharedMemData = ClientRef->parseRequestsBasic(CientSharedMemData);
                ++ProcessedClients;
            }
        }
    }

    //- trigger data processing if processed > 0
    if (ProcessedClients > 0) {
        DBG(100, "Processed Clients:" << ProcessedClients << " ShmBase:" << _SharedMemBase);
        new(_SharedMemBase) atomic_int64_t(1);
        new(static_cast<char*>(_SharedMemBase)+sizeof(atomic_int64_t)) uint16_t(ProcessedClients);
    }
}
