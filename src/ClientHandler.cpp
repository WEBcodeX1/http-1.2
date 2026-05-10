#include "ClientHandler.hpp"

#include <cerrno>
#include <memory>

using namespace std;
extern Configuration ConfigRef;

ClientHandler::ClientHandler() :
    ProcessedClients(0),
    BufferMemory(CLIENTS_MAX, SOCKET_RECEIVE_BUFFER_SIZE)
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

void ClientHandler::addClient(const Filedescriptor_t ClientFD)
{
    DBG(70, "Add client with FD:" << ClientFD);

    //- set client connection non blocking
    Socket::makeNonblocking(ClientFD);

    ClientRef_t ClientObj = std::make_shared<Client>(
        ClientFD, BufferMemory.getNextMemPointer()
    );

    Clients.emplace(
        ClientFD, move(ClientObj)
    );

    //- add fd to epoll
    EpollEvent.events = EPOLLIN | EPOLLET;
    EpollEvent.data.fd = ClientFD;

    epoll_ctl(EpollFD, EPOLL_CTL_ADD, ClientFD, &EpollEvent);
}

void ClientHandler::processClients()
{
    //- reset processed clients count
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

    //- process all filedescriptors with data
    for (uint16_t i=0; i<FDCount; ++i) {

        Filedescriptor_t ReadFD = EpollEvents[i].data.fd;

        if (Clients.contains(ReadFD)) {
            const auto RecvStatus = Clients[ReadFD]->receiveData();
            const int RecvErrno = errno;
            if (RecvStatus == 0) {
                Clients.erase(ReadFD);
                close(ReadFD);
            } else if (
                RecvStatus < 0 &&
                RecvErrno != EAGAIN &&
                RecvErrno != EWOULDBLOCK &&
                RecvErrno != EINTR
            ) {
                Clients.erase(ReadFD);
                close(ReadFD);
            }
        }
    }

    //- trigger data processing in ResultProcessor
    if (ProcessedClients > 0) {
        DBG(100, "Processed Clients:" << ProcessedClients);
    }
}
