#ifndef LibHTTP_parser_hpp
#define LibHTTP_parser_hpp
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
    "Request-UUID",
    "Transfer-Encoding",
    "ETag",
    "Cache-Control",
    "Content-Type",
    "Content-Length"
};


class HTTPParser: private Client, private SHMStaticFS, private SHMPythonAS
{

public:

    HTTPParser(ClientFD_t);
    ~HTTPParser();

    void appendBuffer(const char*, const uint16_t);
    size_t processRequests(SharedMemAddress_t, const ASRequestHandlerRef_t);

private:

    void _splitRequests();
    void _processRequestProperties(const size_t, const ASRequestHandlerRef_t);

    RequestHeader_t _RequestHeaders;
    vector<string> _SplittedRequests;

    size_t _RequestCount;
    size_t _RequestCountGet;
    size_t _RequestCountPost;
    size_t _RequestCountPostAS;

    string _HTTPRequestBuffer;

protected:

    void _parseRequestProperties(string&, BasePropsResultRef_t);
    void _parseRequestHeaders(string&, RequestHeaderResultRef_t);

};

#endif
