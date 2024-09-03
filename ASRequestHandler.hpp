#ifndef ASRequestHandler_hpp
#define ASRequestHandler_hpp

#include "Debug.cpp"
#include "Configuration.hpp"
#include "IPCHandlerAS.hpp"
#include "Vector.hpp"

#include <atomic>
#include <memory>
#include <unordered_map>

typedef struct {
    string VirtualHost;
    uint16_t ClientFD;
    uint8_t HTTPVersion;
    uint16_t RequestNr;
    string Payload;
} RequestProps_t;

typedef uint16_t AppServerID_t;

typedef struct {
    AppServerID_t OffsetStart;
    AppServerID_t OffsetEnd;
} OffsetStartEnd_t;

typedef Vector<RequestProps_t> RequestPropsQueue_t;

typedef unordered_map<string, OffsetStartEnd_t> VHostOffsets_t;
typedef pair<string, OffsetStartEnd_t> PairVHostOffsets_t;

typedef unordered_map<string, vector<AppServerID_t>> VHostOffsetsPrecalc_t;
typedef pair<string, vector<AppServerID_t>> PairVHostOffsetsPrecalc_t;

typedef unordered_map<AppServerID_t, RequestProps_t> Requests_t;
typedef pair<const AppServerID_t, const RequestProps_t> PairRequest_t;


class ASRequestHandler : public SHMPythonAS
{

public:

    ASRequestHandler(Namespaces_t);
    ~ASRequestHandler();

    void addRequest(const RequestProps_t);
    VHostOffsetsPrecalc_t getOffsetsPrecalc();
    uint16_t processQueue();

private:

    void _calculateOffsets();
    void _resetSharedMem();
    AppServerID_t _getNextFreeAppServerID(string);

    Namespaces_t _Namespaces;
    Requests_t _Requests;
    RequestPropsQueue_t _Queue;
    VHostOffsets_t _VHostOffsets;
    VHostOffsetsPrecalc_t _VHostOffsetsPrecalc;

};

//typedef std::shared_ptr<ASRequestHandler> ASRequestHandlerRef_t;
typedef ASRequestHandler* ASRequestHandlerRef_t;

#endif
