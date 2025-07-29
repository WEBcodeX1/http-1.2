#ifndef resultorder_hpp
#define resultorder_hpp

#include "stdint.h"

#include <map>
#include <ctime>
#include <chrono>
#include <vector>
#include <unordered_map>

#include "Global.hpp"
#include "ThreadHandler.hpp"

using namespace std;

typedef map<RequestNr_t, const RequestProps_t> MapReqNrProps_t;
typedef pair<const RequestNr_t, const RequestProps_t> PairReqNrProps_t;
typedef unordered_map<ClientFD_t, MapReqNrProps_t> MapRequests_t;
typedef pair<const ClientFD_t, const MapReqNrProps_t> PairRequests_t;

//- LastRequest

typedef struct {
    RequestNr_t RequestNr;
    time_t RequestTime;
} LastRequestProps_t;

typedef unordered_map<ClientFD_t, LastRequestProps_t> MapLastRequest_t;
typedef pair<ClientFD_t, LastRequestProps_t> PairLastRequest_t;

//- LastProcessed

typedef vector<RequestNr_t> RequestNumbers_t;
typedef unordered_map<ClientFD_t, RequestNumbers_t> MapLastProcessed_t;
typedef pair<ClientFD_t, RequestNumbers_t> PairLastProcessed_t;


class ResultOrder : private ThreadHandler
{

public:

    ResultOrder();
    ~ResultOrder();

    void reset();

    void THsetGlobalData(pidfd_t, Namespaces_t);
    void THprocessThreads();

    void append(const RequestNr_t, RequestProps_t);
    void calculate();
    void processRequests(const HTTPType_t);

private:

    MapRequests_t _Requests;
    MapLastRequest_t _LastRequest;
    MapLastProcessed_t _LastProcessed;

};

#endif
