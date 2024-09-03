#ifndef LibHTTP_parser_hpp
#define LibHTTP_parser_hpp

#include <boost/regex.hpp>

#include "../../Debug.cpp"
#include "../../Helper.hpp"
#include "../../IPCHandler.hpp"
#include "../../IPCHandlerAS.hpp"
#include "../../ASRequestHandler.hpp"
#include "../../Client.hpp"

#include <string>

typedef pair<string, string> HeaderPair_t;
typedef unordered_map<string, string> RequestHeader_t;

typedef RequestHeader_t RequestHeaderResult_t;
typedef RequestHeader_t& RequestHeaderResultRef_t;

typedef vector<string> BasePropsResult_t;
typedef BasePropsResult_t& BasePropsResultRef_t;


static const vector<string> HeaderList
{
    "Host",
    "Request-UUID"
    "User-Agent",
    "ETag"
};


class HTTPParser: private Client, private SHMStaticFS, private SHMPythonAS
{

public:

    HTTPParser(ClientFD_t);
    ~HTTPParser();

    void appendBuffer(const char*, const uint16_t);
    uint parseRequestsBasic(SharedMemAddress_t, const ASRequestHandlerRef_t);
    void parseRequestsComplete();

private:

    void _splitRequests();
    void _parseRequestHeader();
    uint16_t _parseBaseProps(string&, const ASRequestHandlerRef_t);

    RequestHeader_t _RequestHeaders;
    vector<string> _SplittedRequests;

    size_t _RequestCount;
    string _HTTPRequest;

protected:

    void _parseBasePropsRV(string&, BasePropsResultRef_t);
    void _parseHeadersRV(string&, RequestHeaderResultRef_t);

};

#endif
