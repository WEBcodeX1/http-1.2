#include "ClientHandler.hpp"

#include <memory>

using namespace std;
extern Configuration ConfigRef;

ClientHandler::ClientHandler() :
    ProcessedClients(0),
    BufferMemory(CLIENTS_MAX, TMP_BUFFER_BYTES)
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

void ClientHandler::setSharedMemPointer(ClientHandlerSHMPointer_t SharedMemPointer) {
    _SHMStaticFS = SharedMemPointer.StaticFSPtr;
    _SHMPythonASMeta = SharedMemPointer.PostASMetaPtr;
    _SHMPythonASRequests = SharedMemPointer.PostASRequestsPtr;
    _SHMPythonASResults = SharedMemPointer.PostASResultsPtr;
}

void ClientHandler::setClientHandlerConfig() {
    _ASRequestHandlerRef = std::make_unique<ASRequestHandler>(
        ConfigRef.Namespaces,
        BaseAdresses_t{ _SHMPythonASMeta, _SHMPythonASRequests, _SHMPythonASResults }
    );
}

ASRequestHandler& ClientHandler::getClientHandlerASRequestHandlerRef() {
    return *_ASRequestHandlerRef;
}

void ClientHandler::addClient(const uint16_t ClientFD)
{
    DBG(70, "Add client with FD:" << ClientFD);

    //- set client connection non blocking
    Socket::makeNonblocking(ClientFD);

    ClientRef_t ClientObj = std::make_shared<HTTPParser>(ClientFD, ConfigRef.Namespaces);

    Clients.emplace(
        ClientFD, ClientObj
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

    //- process appserver queue
    ProcessedClients += _ASRequestHandlerRef->processQueue();
}

void ClientHandler::readClientData(const uint16_t FDCount)
{
    DBG(70, "Read client data. Filedescriptor count:" << FDCount);

    //- set offset starting addresses
    void* SHMGetRequests = static_cast<char*>(_SHMStaticFS) + sizeof(atomic_uint16_t) + sizeof(uint16_t);

    DBG(100, "Parent Server GetRequestsSHM Address:" << SHMGetRequests);

    //- process all filedescriptors with data (or close)
    for (uint16_t i=0; i<FDCount; ++i) {

        uint16_t ReadFD = EpollEvents[i].data.fd;

        // read data into buffer
        char* Buffer = BufferMemory.getNextMemPointer();
        uint16_t RcvBytes = read(ReadFD, Buffer, TMP_BUFFER_BYTES);
        DBG(220, "RcvBytes:" << RcvBytes << " ReadFD:" << ReadFD);

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
                ClientRef_t ClientRef = Clients[ReadFD];
                ClientRef->appendBuffer(Buffer, RcvBytes);

                if (ClientRef->processRequests(SHMGetRequests, _ASRequestHandlerRef) > 0) {
                    ++ProcessedClients;
                }
            }
        }
    }

    //- trigger data processing in ResultProcessor
    if (ProcessedClients > 0) {
        DBG(100, "Processed Clients:" << ProcessedClients << " ShmBase:" << _SHMStaticFS);
        new(_SHMStaticFS) atomic_uint16_t(1);
        new(static_cast<char*>(_SHMStaticFS)+sizeof(atomic_uint16_t)) uint16_t(ProcessedClients);
    }
}
