#ifndef ThreadHandler_hpp
#define ThreadHandler_hpp

#include "Debug.cpp"

#include <sys/time.h>
#include <sys/sendfile.h>

#include <unordered_map>
#include <iostream>
#include <iomanip>
#include <ctime>
#include <chrono>
#include <thread>

#include "./lib/http/httpparser.hpp"

#include "Helper.hpp"
#include "Global.hpp"
#include "Configuration.hpp"


using namespace std;

typedef struct {
    ClientFD_t ClientFD;
    ClientFD_t ClientFDShared;
    uint16_t MsgLength;
    string HTTPMessage;
} ClientRequestData_t;

typedef thread ClientPThread_t;
typedef vector<ClientRequestData_t> ClientRequestDataVec_t;
typedef unordered_map<ClientFD_t, ClientRequestDataVec_t> ClientRequestDataList_t;

typedef struct {
    pidfd_t ParentPidFD;
    Namespaces_t Namespaces;
} ThreadHandlerGlobals_t;


class ClientThread: private HTTPParser
{

public:

    ClientThread(ClientRequestDataVec_t, ThreadHandlerGlobals_t);
    ~ClientThread();

    void startPocessingThread();
    void processRequests();
    void processRequest(unsigned int);

    bool join();

private:

    ClientPThread_t _ThreadRef;
    ClientRequestDataVec_t _ClientRequests;
    ThreadHandlerGlobals_t _Globals;

};


typedef const std::shared_ptr<ClientThread> ClientThreadObjRef_t;
typedef pair<const ClientFD_t, const ClientThreadObjRef_t> ClientListPair_t;
typedef pair<const ClientFD_t, vector<ClientRequestData_t>> ClientRequestDataPair_t;

typedef unordered_map<ClientFD_t, const ClientThreadObjRef_t> ClientThreads_t;


class ThreadHandler
{

public:

    ThreadHandler();
    ~ThreadHandler();

    friend class StaticFS; //- TODO: correct?

private:

    void _setGlobalData(pidfd_t, Namespaces_t);
    void _addClient(ClientRequestData_t);
    void _startProcessingThreads();
    void _checkProcessed();

    ThreadHandlerGlobals_t _Globals;
    ClientRequestDataList_t _ClientRequests;
    ClientThreads_t _ClientThreads;
};

#endif
