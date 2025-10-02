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

#include "../lib/http/httpparser.hpp"

#include "Helper.hpp"
#include "Global.hpp"
#include "Configuration.hpp"


using namespace std;

typedef thread ClientPThread_t;
typedef uint16_t PocessRequestsIndex_t;

typedef vector<RequestProps_t> ClientRequestDataVec_t;
typedef unordered_map<ClientFD_t, ClientRequestDataVec_t> ClientRequestDataList_t;
typedef pair<ClientFD_t, ClientRequestDataVec_t> ClientRequestDataListPair_t;
typedef vector<ClientRequestDataListPair_t> ClientRequestDataListVector_t;
typedef unordered_map<ClientFD_t, PocessRequestsIndex_t> ProcessRequestsIndexList_t;
typedef pair<ClientFD_t, PocessRequestsIndex_t> ProcessRequestsIndexListPair_t;


class ClientThread: private HTTPParser
{

public:

    ClientThread(
        ClientFD_t, Namespaces_t&, ClientRequestDataVec_t
    );

    ~ClientThread();

    void startThread();
    void processRequests();

    bool join();

private:

    ClientPThread_t _ThreadRef;

    ClientFD_t _ClientFD;
    ClientFD_t _ClientFDShared;
    Namespaces_t& _Namespaces;

    ClientRequestDataVec_t _ClientRequests;

};


typedef const std::shared_ptr<ClientThread> ClientThreadObjRef_t;
typedef pair<const ClientFD_t, const ClientThreadObjRef_t> ClientListPair_t;
typedef unordered_map<ClientFD_t, const ClientThreadObjRef_t> ClientThreads_t;


class ThreadHandler
{

public:

    ThreadHandler();
    ~ThreadHandler();

protected:

    void _addRequests(ClientRequestDataVec_t);
    void _processThreads();
    void _checkProcessed();

    ClientRequestDataList_t _RequestsSorted;
    ClientRequestDataListVector_t _ProcessRequests;
    ProcessRequestsIndexList_t _ProcessRequestsIndex;
    ClientThreads_t _ClientThreads;
};

#endif
