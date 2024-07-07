#include "httpparser.hpp"

using namespace std;


HTTPParser::HTTPParser(ClientFD_t ClientFD) :
    Client(ClientFD),
    _RequestCount(0),
    _HTTPRequest("")
{
    DBG(120, "Constructor");
}

HTTPParser::~HTTPParser()
{
    DBG(120, "Destructor");
}

void HTTPParser::appendBuffer(const char* BufferRef, const uint16_t BufferSize)
{
    _HTTPRequest = _HTTPRequest + string(&BufferRef[0], BufferSize);
    _splitRequests();
}

void HTTPParser::_splitRequests()
{
    DBG(180, "splitRequests Buffer:'" << _HTTPRequest << "'");
    _SplttedRequests.clear();
    String::split(_HTTPRequest, "\n\r", _SplttedRequests);
    _RequestCount = _SplttedRequests.size();
    DBG(120, "splitRequests after split Vector Element count:" << _RequestCount);

    //-> cut "complete" requests from buffer including last \n\r
    size_t LastDelimiterPos = _HTTPRequest.rfind("\n\r");
    _HTTPRequest = _HTTPRequest.replace(0, LastDelimiterPos+2, "");
}

void HTTPParser::parseRequestsComplete()
{
    for (auto Request:_SplttedRequests) {
        vector<string> RequestLines;
        String::split(Request, "\n", RequestLines);
    }
}

SharedMemAddress_t HTTPParser::parseRequestsBasic(void* SharedMemAddress) {
    setSharedMemBaseAddress(SharedMemAddress);
    for (auto Request:_SplttedRequests) {
        _parseBaseProps(Request);
    }
    return getCurrentOffsetAddress();
}

void HTTPParser::_parseBaseProps(string& Request) //- rename to "_parseBasePropsSHM"
{
    DBG(180, "HTTP Request:'" << Request << "'");

    //- find first line endline
    size_t StartPos = Request.find("\n");

    //- set result vector
    vector<string> BasePropsFound;

    //- reverse split
    String::rsplit(Request, StartPos, " ", BasePropsFound);

    DBG(180, "HTTP Version:" << BasePropsFound.at(0) << " File:" << BasePropsFound.at(1) << " Method:" << BasePropsFound.at(2));
    DBG(180, "HTTP Message (c_str):" << Request.c_str());

    //- set values in shared memory
    const char* MsgCString = Request.c_str();

    uint16_t MsgLength = Request.length();

    void* ClientFDAddr = getCurrentOffsetAddress();
    void* MsgLengthAddr = getNextSharedMemAddress();

    new(ClientFDAddr) ClientFD_t(_ClientFD);
    new(MsgLengthAddr) uint16_t(MsgLength);

    void* MsgAddress = getNextSharedMemAddress();
    memcpy(MsgAddress, &MsgCString[0], MsgLength);

    void* NextSegmentAddr = getNextSharedMemAddress(MsgLength);
    DBG(120, "Set SharedMem ClientFD:" << ClientFDAddr << " MsgLength:" << MsgLengthAddr << " Msg:" << MsgAddress << " NextSegment:" << NextSegmentAddr);
}

void HTTPParser::_parseBasePropsRV(string& Request, BasePropsResultRef_t ResultRef)
{
    DBG(120, "HTTP Request:'" << Request << "'");

    //- find first line endline
    size_t StartPos = Request.find("\n");

    //- reverse split
    String::rsplit(Request, StartPos, " ", ResultRef);

    //- remove first line from Request
    Request = Request.substr(StartPos+1, Request.length());

    DBG(120, "HTTP Version:" << ResultRef.at(0) << " File:" << ResultRef.at(1) << " Method:" << ResultRef.at(2) << " Request:" << Request);
}

void HTTPParser::_parseHeadersRV(string& Request, RequestHeaderResultRef_t ResultRef)
{
    DBG(120, "HTTP Request:'" << Request << "'");

    //- reverse split header lines
    vector<string> Lines;
    String::split(Request, "\n", Lines);

    //- loop over lines, split, put into result map
    for (auto Line:Lines) {

        DBG(120, "Line:'" << Line << "'");

        vector<string> HeaderPair;
        String::rsplit(Line, Line.length(), ": ", HeaderPair);

        string HeaderID = HeaderPair.at(1);
        string HeaderValue = HeaderPair.at(0).substr(0, HeaderPair.at(0).length()-1);

        DBG(120, "HeaderID:'" << HeaderID << "'");
        DBG(120, "HeaderValue:'" << HeaderValue << "'");

        ResultRef.insert(
            HeaderPair_t(HeaderID, HeaderValue)
        );
    }
}
