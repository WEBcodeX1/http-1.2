#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <memory>
#include <cstdint>
#include <ostream>
#include <unordered_map>
using namespace std;

//- Header fields map: key = element name (e.g. "Host", "URL", "Mime-Type"), value = text content
typedef unordered_map<string, string> NLAPHeader_t;
typedef NLAPHeader_t& NLAPHeaderRef_t;


// ---------------------------------------------------------------------------
//  Zero-copy message segment: pointer + length into the original buffer
// ---------------------------------------------------------------------------

struct MessageSegment
{
    const char* start;   //- start address of the message in the buffer
    size_t      length;  //- byte length of the complete message (including tags)
};


// ---------------------------------------------------------------------------
//  Result of multi-message framing (zero-copy mode)
// ---------------------------------------------------------------------------

enum class FrameStatus : uint8_t
{
    Valid,           //- all messages are well-framed and contiguous
    Invalid,         //- garbage, double tags, or non-contiguous data found
    Incomplete       //- trailing partial message (valid prefix, incomplete tail)
};

//- output operator for FrameStatus (required by Boost.Test BOOST_TEST macro)
inline ostream& operator<<(ostream& os, FrameStatus s)
{
    switch (s) {
        case FrameStatus::Valid:      return os << "Valid";
        case FrameStatus::Invalid:    return os << "Invalid";
        case FrameStatus::Incomplete: return os << "Incomplete";
    }
    return os << "Unknown(" << static_cast<int>(s) << ")";
}

struct MessageFrameResult
{
    FrameStatus              status;
    uint16_t                 message_count;
    vector<MessageSegment>   messages;
    size_t                   bytes_consumed;  //- total bytes consumed from the buffer
};


// ---------------------------------------------------------------------------
//  Unified request/response properties covering all NLAP sub-protocols:
//-   NLAMP  (Next Level Application Metadata Protocol)
//-   NLAFP  (Next Level Application File Protocol)
//-   NLASP  (Next Level Application Session Protocol)
//-   NLAPP  (Next Level Application Proxy Protocol)
//-   NLAPS  (Next Level Application Protocol Secure Extension)
// ---------------------------------------------------------------------------

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

    //- Zero-copy message framing: scan a buffer (or string_view) for
    //- complete <NLAP>...</NLAP> messages and return their locations.
    //- No XML parsing is performed; only message boundary detection.
    static MessageFrameResult frameMessages(const char* buffer, size_t length);
    static MessageFrameResult frameMessages(string_view buffer);

private:

    void _processRequests();
    bool _processRequestProperties(const size_t Index);
    bool _parseXML(string_view XMLMessage, RequestProperties_t& Props);

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
