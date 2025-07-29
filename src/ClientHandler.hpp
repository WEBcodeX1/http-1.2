#ifndef ClientHandler_hpp
#define ClientHandler_hpp

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
#include "ASRequestHandler.hpp"

#include "../lib/http/httpparser.hpp"


typedef std::shared_ptr<HTTPParser> ClientRef_t;
typedef pair<uint16_t, const ClientRef_t> ClientMapPair_t;
typedef unordered_map<uint16_t, const ClientRef_t> ClientMap_t;

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
    void setSharedMemPointer(ClientHandlerSHMPointer_t);
    void setClientHandlerConfig(Namespaces_t);

    ASRequestHandlerRef_t getClientHandlerASRequestHandlerRef();

    uint16_t ProcessedClients;
    MemoryManager<char> BufferMemory;

private:

    ClientMap_t Clients;

    struct epoll_event EpollEvent, EpollEvents[EPOLL_FD_COUNT_MAX];

    int EpollFD;

    uint8_t LastProcessingIDStaticFS;
    uint8_t LastProcessingIDAppServer;

    void* _SHMStaticFS;
    void* _SHMPythonASMeta;
    void* _SHMPythonASRequests;
    void* _SHMPythonASResults;

    ASRequestHandlerRef_t _ASRequestHandlerRef;

    Namespaces_t _Namespaces;

};

#endif
