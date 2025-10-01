#include "Server.hpp"

using namespace std;

static bool RunServer = true;
Configuration ConfigRef = Configuration();

std::vector<pid_t> Server::ChildPIDs;

//- global function for ASProcessHandler to register child PIDs
void registerChildPIDToServer(pid_t pid)
{
    Server::addChildPID(pid);
}


Server::Server() :
    SocketListenAddress("127.0.0.1"),
    SocketListenPort(80)
{
    DBG(120, "Constructor");
}

Server::~Server()
{
    DBG(120, "Destructor");
}

void Server::init()
{
    //- setup shared memory
    setupSharedMemory();
    setSharedMemPointer( { _SHMStaticFS, _SHMPythonASMeta, _SHMPythonASRequests, _SHMPythonASResults } );

    //- init static filesystem
    ConfigRef.mapStaticFSData();

    //- set client handler namespaces
    setClientHandlerConfig();

    //- TODO: just set if exists in config, else default
    SocketListenAddress = ConfigRef.ServerAddress;
    SocketListenPort = ConfigRef.ServerPort;

    //- disable OS signals SIGINT, SIGPIPE
    Signal::disableSignals();

    #if defined(DEBUG_BUILD)
    //- overwrite termination handler (display backtrace)
    std::set_terminate(SigHandler::myterminate);
    #endif

    //- setup termination handler
    setTerminationHandler();

    //- setup server socket
    setupSocket();

    //- setup server socket monitoring
    setupPoll();

    //- get ASRequestHandler reference
    ASRequestHandler& ASRequestHandlerRef = getClientHandlerASRequestHandlerRef();

    //- fork result processor process
    ResultProcessor::setVHostOffsets(ASRequestHandlerRef.getOffsetsPrecalc());
    pid_t resultProcessorPID = ResultProcessor::forkProcessResultProcessor( { _SHMStaticFS, _SHMPythonASMeta, _SHMPythonASRequests, _SHMPythonASResults } );
    if (resultProcessorPID > 0) {
        addChildPID(resultProcessorPID);
    }

    //- fork application server processes
    setASProcessHandlerOffsets(ASRequestHandlerRef.getOffsetsPrecalc());
    forkProcessASHandler( { _SHMPythonASMeta, _SHMPythonASRequests, _SHMPythonASResults } );

    //- check interpreter count
    uint ASInterpreterCount = getASInterpreterCount();
    DBG(50, "Sum AS Interpreters:" << ASInterpreterCount);

    //- apply cpu bound processing
    //setCPUConfig();

    //- drop privileges
    Permission::dropPrivileges(ConfigRef.RunAsUnixGroupID, ConfigRef.RunAsUnixUserID);

    //- start server loop
    ServerLoop();
}

void Server::setTerminationHandler()
{
    DBG(-1, "Setting Main Server SIGTERM handler.");
    signal(SIGTERM, Server::terminate);
}

void Server::addChildPID(pid_t pid)
{
    ChildPIDs.push_back(pid);
    DBG(120, "Registered child PID:" << pid);
}

void Server::terminateChildren()
{
    DBG(-1, "Sending SIGTERM to " << ChildPIDs.size() << " child processes");
    reverse(ChildPIDs.begin(), ChildPIDs.end());
    for (const auto& pid : ChildPIDs) {
        DBG(-1, "Sending SIGTERM to child PID:" << pid);
        kill(pid, SIGTERM);
        this_thread::sleep_for(chrono::milliseconds(50));
    }
}

void Server::terminate(int _ignored)
{
    DBG(-1, "SIGTERM Main Server received, shutting down");
    terminateChildren();
    RunServer = false;
}

void Server::setupSocket()
{
    DBG(60, "Setting up TCP listening Socket/Server.");

    try
    {
        //- setup socket, parameter
        ServerSocketFD = socket(AF_INET, SOCK_STREAM, 0);

        memset((char*)&SocketAddr, 0, sizeof(SocketAddr));

        SocketAddr.sin_family = AF_INET;
        SocketAddr.sin_port = htons(SocketListenPort);
        SocketAddr.sin_addr.s_addr = inet_addr(SocketListenAddress.c_str());

        //- set socket reuse address flag
        int flags = fcntl(ServerSocketFD, F_GETFL, 0);

        if (fcntl(ServerSocketFD, F_SETFL, flags | SO_REUSEADDR) < 0) {
            cout << "Error setting Socket flag SO_REUSEADDR." << endl;
            exit(EXIT_FAILURE);
        }

        const int sockopt = 1;

        //- set socket TCP_CORK
        //setsockopt(ServerSocketFD, IPPROTO_TCP, TCP_CORK, &sockopt, sizeof(int));

        //- set socket TCP_NODELAY
        setsockopt(ServerSocketFD, IPPROTO_TCP, TCP_NODELAY, &sockopt, sizeof(int));

        //- make server socket nonblocking
        Socket::makeNonblocking(ServerSocketFD);

        //- bind socket, start listen
        int rc = bind(ServerSocketFD, (struct sockaddr*)&SocketAddr, sizeof(SocketAddr));

        if (rc != 0) {
            cout << "Error binding Socket." << endl;
            exit(EXIT_FAILURE);
        }

        listen(ServerSocketFD, 0);
    }
    catch( const char* msg )
    {
        ERR("Setting up listening Socket/Server failed:" << msg);
        exit(1);
    }
}

void Server::setupPoll()
{
    ServerConnFD[0].fd = ServerSocketFD;
    ServerConnFD[0].events = POLLIN;
}

void Server::ServerLoop()
{
    DBG(-1, "Network processing runs on CPU(s) mask:" << sched_getcpu());

    //- main server loop
    while(RunServer) {

        //- poll server fd for incoming connections
        int rc;

        DBG(300, "Server loop, poll server fd for incoming connections");
        rc = poll(ServerConnFD, 1, 0);

        if (rc == -1) {
            ERR("Server Socket poll Error:" << errno << " ErrorString:" << strerror(errno));
        }
        else {
            DBG(300, "Server poll revents:" << ServerConnFD[0].revents);

            //- check for incoming connection
            if (ServerConnFD[0].revents & POLLIN) {
                acceptClient();
            }
            //- no new client and no processed clients (idle)
            else if (ProcessedClients == 0) {
                this_thread::sleep_for(chrono::microseconds(IDLE_SLEEP_MICROSECONDS));
            }
        }

        //- process clients
        processClients();
    }

    DBG(10, "Server exit");
}

void Server::acceptClient()
{
    socklen_t ClientSocketLen;
    ClientSocketLen = sizeof(ClientSocketAddr);

    int ClientFD = accept(
        ServerSocketFD,
        reinterpret_cast<struct sockaddr*>(&ClientSocketAddr),
        &ClientSocketLen
    );

    if (ClientFD > 0) {
        DBG(300, "Accepted client fd:" << ClientFD);
        addClient(ClientFD);
    }
    else {
        ERR("Accept client fd failed Err:" << strerror(errno));
    }
}

void Server::setupSharedMemory()
{
    DBG(120, "Setup Shared Memory.");

    _SHMStaticFS = mmap(NULL, SHMEM_STATICFS_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS | MAP_HUGETLB, -1, 0);
    madvise(_SHMStaticFS, SHMEM_STATICFS_SIZE, MADV_HUGEPAGE);

    _SHMPythonASMeta = mmap(NULL, SHMEM_STATICFS_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS | MAP_HUGETLB, -1, 0);
    madvise(_SHMPythonASMeta, SHMEM_STATICFS_SIZE, MADV_HUGEPAGE);

    _SHMPythonASRequests = mmap(NULL, SHMEM_STATICFS_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS | MAP_HUGETLB, -1, 0);
    madvise(_SHMPythonASRequests, SHMEM_STATICFS_SIZE, MADV_HUGEPAGE);

    _SHMPythonASResults = mmap(NULL, SHMEM_STATICFS_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS | MAP_HUGETLB, -1, 0);
    madvise(_SHMPythonASResults, SHMEM_STATICFS_SIZE, MADV_HUGEPAGE);

    DBG(120, "SharedMemAddress:" << _SHMStaticFS);
}
