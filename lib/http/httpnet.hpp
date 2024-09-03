#ifndef LibHTTP_net_hpp
#define LibHTTP_net_hpp

#include <boost/regex.hpp>

#include "../../Debug.cpp"
#include "../../Helper.hpp"

#include <string>

/*
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
    "Last-modified", //- remove, just in response!
    "ETag"
};
*/

class HTTPNet
{

public:

    HTTPNet();
    ~HTTPNet();

    /*
    void appendBuffer(const char*, const uint16_t);
    SharedMemAddress_t parseRequestsBasic(void*);
    void parseRequestsComplete();
    */

private:

    /*
    void _splitRequests();
    void _parseRequestHeader();
    void _parseBaseProps(string&);

    RequestHeader_t _RequestHeaders;
    vector<string> _SplttedRequests;

    size_t _RequestCount;
    string _HTTPRequest;
    */
protected:
    /*
    void _parseBasePropsRV(string&, BasePropsResultRef_t);
    void _parseHeadersRV(string&, RequestHeaderResultRef_t);
    */
};

#endif
