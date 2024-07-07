#ifndef Server_hpp
#define Server_hpp

#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

#include <poll.h>
#include <thread>

#include "Global.hpp"
#include "Constant.hpp"
#include "Configuration.hpp"
#include "ClientHandler.hpp"
#include "StaticFS.hpp"


class Server : public Configuration, public StaticFS, public ClientHandler {

public:

    Server();
    ~Server();

    void setupSocket();
    void setupPoll();
    void dropPrivileges();
    void ServerLoop();
    void acceptClient();

    void setTerminationHandler();
    static void terminate(int);

    void* setupSharedMemory();
    void* getSharedMemPointer();

private:

    string SocketListenAddress;
    uint SocketListenPort;

    struct sockaddr_in SocketAddr;
    struct sockaddr_in ClientSocketAddr;

    int ServerSocketFD;

    struct pollfd ServerConnFD[1];

};

#endif
