#pragma once

#include <string>
#include <vector>
#include <memory>
#include <cstdint>
#include <unordered_map>
using namespace std;

//- Header fields map: key = element name (e.g. "Host", "URL", "Mime-Type"), value = text content
typedef unordered_map<string, string> NLAPHeader_t;
typedef NLAPHeader_t& NLAPHeaderRef_t;

//- Unified request/response properties covering all NLAP sub-protocols:
//-   NLAMP  (Next Level Application Metadata Protocol)
//-   NLAFP  (Next Level Application File Protocol)
//-   NLASP  (Next Level Application Session Protocol)
//-   NLAPP  (Next Level Application Proxy Protocol)
//-   NLAPS  (Next Level Application Protocol Secure Extension)
struct RequestProperties_t
{
    //- full, valid, parsable XML NLAP message (beginning with <?xml)
    string XMLRawMessage;

    //- common fields (Request and Response)
    string RequestType;     //- "Request" or "Response"
    string UUID;
    string Protocol;
    string Version;
    string Subtype;

    //- Header child elements (all optional per DTD; stored by element name)
    //- Covers NLAMP: Host, URL, User-Agent, Mime-Type, Encoding
    //- Covers NLAFP additions: Byte-Size, Byte-Size-Full, Byte-Size-Part,
    //-                         Compression, File-UUID, File-Part-Sum, File-Part
    //- Covers NLAPS additions: user, Connection-Close
    NLAPHeader_t Header;

    //- Security block (NLAPS)
    string Encryption;
    string Signature;

    //- Payload (optional, ANY content)
    string Payload;

    //- Status block (Response only)
    string StatusCode;
    string StatusDescription;
    string StatusException;
};

typedef RequestProperties_t& RequestPropertiesRef_t;
typedef shared_ptr<RequestProperties_t> RequestPropertiesPtr_t;

typedef unordered_map<uint16_t, RequestProperties_t> RequestsMap_t;


class XMLParser
{

public:

    XMLParser(const uint16_t BufferSize);
    ~XMLParser();

    void appendBuffer(const char* BufferRef, const uint16_t RecvBytes);
    RequestsMap_t getRequests();
    RequestPropertiesPtr_t getNextRequest();
    void removeRequest(uint16_t Index);

private:

    void _processRequests();
    bool _processRequestProperties(const size_t Index);
    bool _parseXML(const string& XMLMessage, RequestProperties_t& Props);

    vector<string> _SplittedRequests;

    uint16_t _RequestParseError;

    uint16_t _ReqAddIndex;
    uint16_t _ReqNextIndex;

    string _XMLRequestBuffer;
    uint16_t _XMLRequestBufferMax;

    RequestProperties_t _RequestProperties;
    RequestsMap_t _Requests;

    void* _grammarPool;     //- xercesc::XMLGrammarPool* (opaque to avoid Xerces in public header)
};
