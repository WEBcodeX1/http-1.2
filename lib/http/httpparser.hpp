#ifndef LibHTTP_parser_hpp
#define LibHTTP_parser_hpp
#include "../../src/Debug.cpp"
#include "../../src/Helper.hpp"
#include "../../src/IPCHandler.hpp"
#include "../../src/IPCHandlerAS.hpp"
#include "../../src/ASRequestHandler.hpp"
#include "../../src/Client.hpp"

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
    "If-None-Match",
    "Content-Type",
    "Content-Length"
};


class HTTPParser: private Client, private SHMStaticFS, private SHMPythonAS
{

public:

    HTTPParser(const ClientFD_t, const NamespacesRef_t);
    ~HTTPParser();

    void appendBuffer(const char*, const uint16_t);
    size_t processRequests(SharedMemAddress_t, const ASRequestHandlerRef_t&);

private:

    inline void _splitRequests();
    void _processRequestProperties(const size_t, const ASRequestHandlerRef_t&);

    RequestHeader_t _RequestHeaders;
    vector<string> _SplittedRequests;

    NamespacesRef_t _Namespaces;

    size_t _RequestCount;
    size_t _RequestCountGet;
    size_t _RequestCountPost;
    size_t _RequestCountPostAS;

    uint16_t _RequestNumber;

    string _HTTPRequestBuffer;

protected:

    void _parseRequestProperties(string&, BasePropsResultRef_t);
    void _parseRequestHeaders(string&, RequestHeaderResultRef_t);

    inline string _getASURLParamValue(
        const string&,
        const uint16_t,
        string&
    );

    inline void _processASPayload(
        const ASRequestHandlerRef_t&,
        const RequestHeaderResult_t&,
        const uint16_t,
        const uint16_t,
        const uint16_t,
        const string&
    );
};

#endif
