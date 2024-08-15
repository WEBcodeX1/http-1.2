#ifndef resultorder_hpp
#define resultorder_hpp

#include "stdint.h"

#include <map>
#include <ctime>
#include <chrono>
#include <vector>
#include <unordered_map>

using namespace std;

typedef uint8_t HTTPType_t;
typedef uint16_t ASIndex_t;
typedef uint16_t RequestNr_t;
typedef uint32_t ClientFD_t;
typedef uint32_t SendfileFD_t;

//- Requests

typedef struct {
    HTTPType_t HTTPType;
    SendfileFD_t SendfileFD;
    ASIndex_t ASIndex;
} RequestProps_t;

typedef map<RequestNr_t, const RequestProps_t> MapReqNrProps_t;
typedef pair<RequestNr_t, const RequestProps_t> PairReqNrProps_t;
typedef unordered_map<ClientFD_t, MapReqNrProps_t> MapRequests_t;
typedef pair<ClientFD_t, MapReqNrProps_t> PairRequests_t;

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


class ResultOrder
{

public:

    ResultOrder();
    ~ResultOrder();

    void append(const ClientFD_t, const RequestNr_t, const RequestProps_t);

private:

    MapRequests_t _Requests;
    MapLastRequest_t _LastRequest;
    MapLastProcessed_t _LastProcessed;

    void _processClients();

};

#endif
