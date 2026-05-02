#pragma once

#include "Debug.cpp"

#include <sys/socket.h>
#include <sys/time.h>
#include <sys/epoll.h>

#include <ctime>
#include <atomic>
#include <unordered_map>

#include "Helper.hpp"
#include "Global.hpp"
#include "Configuration.hpp"
#include "MemoryManager.hpp"

//typedef std::shared_ptr<Client> ClientRef_t;
//typedef pair<uint16_t, const ClientRef_t> ClientMapPair_t;

typedef Client Client_t;
typedef Client& ClientRef_t;

typedef unordered_map<uint16_t, ClientRef_t> ClientMap_t;
typedef vector<Client_t> ClientVector_t;

typedef struct {
    void* StaticFSPtr;
    void* PostASMetaPtr;
    void* PostASRequestsPtr;
    void* PostASResultsPtr;
} ClientHandlerSHMPointer_t;


class ClientHandler
{

public:

    ClientHandler();
    ~ClientHandler();

    void addClient(const uint16_t);
    void processClients();
    void readClientData(const uint16_t);

    /*
    void setSharedMemPointer(ClientHandlerSHMPointer_t);
    void setClientHandlerConfig();
    ASRequestHandler& getClientHandlerASRequestHandlerRef();
    */

    uint16_t ProcessedClients;
    MemoryManager<char> BufferMemory;

private:

    ClientMap_t Clients;
    ClientVector_t ClientsVector;

    struct epoll_event EpollEvent, EpollEvents[EPOLL_FD_COUNT_MAX];

    int EpollFD;

    uint8_t LastProcessingIDStaticFS;
    uint8_t LastProcessingIDAppServer;

    void* _SHMStaticFS;
    void* _SHMPythonASMeta;
    void* _SHMPythonASRequests;
    void* _SHMPythonASResults;

    //ASRequestHandlerRef_t _ASRequestHandlerRef;

};
