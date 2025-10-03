#ifndef Server_hpp
#define Server_hpp

#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <fcntl.h>
#include <cerrno>

#include <poll.h>
#include <thread>
#include <vector>

#include "Global.hpp"
#include "Constant.hpp"
#include "Configuration.hpp"
#include "ClientHandler.hpp"
#include "ASProcessHandler.hpp"
#include "ResultProcessor.hpp"


class Server : private ResultProcessor, private ASProcessHandler, private ClientHandler {

public:

    Server();
    ~Server();

    void init();
    void setupSocket();
    void setupPoll();
    void ServerLoop();
    void acceptClient();

    void setTerminationHandler();
    static void terminate(int);

    void setupSharedMemory();
    void setupFDPassingServer();
    void handleFDPassingRequests();

    static void addChildPID(pid_t);
    static void terminateChildren();

private:

    string SocketListenAddress;
    uint SocketListenPort;

    struct sockaddr_in SocketAddr;
    struct sockaddr_in ClientSocketAddr;

    int ServerSocketFD;

    struct pollfd ServerConnFD[1];

    void* _SHMStaticFS;
    void* _SHMPythonASMeta;
    void* _SHMPythonASRequests;
    void* _SHMPythonASResults;

    int _FDPassingServerFD;

    static std::vector<pid_t> ChildPIDs;

};

#endif
