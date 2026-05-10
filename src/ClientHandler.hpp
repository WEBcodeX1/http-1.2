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
#include "Client.hpp"
#include "Configuration.hpp"
#include "MemoryManager.hpp"

//- typedefs
typedef Client Client_t;
typedef std::shared_ptr<Client> ClientRef_t;
typedef unordered_map<Filedescriptor_t, ClientRef_t> ClientMap_t;


class ClientHandler
{

public:

    ClientHandler();
    ~ClientHandler();

    void addClient(const uint16_t);
    void processClients();
    void readClientData(const uint16_t);

    uint16_t ProcessedClients;

private:

    MemoryManager<char> BufferMemory;

    ClientMap_t Clients;

    struct epoll_event EpollEvent, EpollEvents[EPOLL_FD_COUNT_MAX];

    int EpollFD;
};
