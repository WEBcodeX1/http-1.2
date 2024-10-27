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
    _HTTPRequest = _HTTPRequest + std::string(&BufferRef[0], BufferSize);
    _splitRequests();
}

void HTTPParser::_splitRequests()
{
    DBG(180, "splitRequests Buffer:'" << _HTTPRequest << "'");
    _SplittedRequests.clear();
    String::split(_HTTPRequest, "\n\r", _SplittedRequests);
    _RequestCount = _SplittedRequests.size();
    DBG(120, "splitRequests after split Vector Element count:" << _RequestCount);

    //-> cut "complete" requests from buffer including last \n\r
    size_t LastDelimiterPos = _HTTPRequest.rfind("\n\r");
    _HTTPRequest = _HTTPRequest.replace(0, LastDelimiterPos+2, "");
}

void HTTPParser::parseRequestsComplete()
{
    for (auto Request:_SplittedRequests) {
        vector<std::string> RequestLines;
        String::split(Request, "\n", RequestLines);
    }
}

uint HTTPParser::parseRequestsBasic(SharedMemAddress_t SHMGetRequests, const ASRequestHandlerRef_t ASRequestHandlerRef)
{
    setBaseAddress(SHMGetRequests);

    uint16_t PythonRequestCount = 0;

    for (auto Request:_SplittedRequests) {
        PythonRequestCount += _parseBaseProps(Request, ASRequestHandlerRef);
    }

    //SHMGetRequests = getCurrentOffsetAddress();

    //return getCurrentOffsetAddress();
    return _SplittedRequests.size() - PythonRequestCount;
}

uint16_t HTTPParser::_parseBaseProps(string& Request, const ASRequestHandlerRef_t ASRequestHandlerRef)
{
    DBG(180, "HTTP Request:'" << Request << "'");

    //- find first line endline
    size_t StartPos = Request.find("\n");

    //- set result vector
    vector<std::string> BasePropsFound;

    //- reverse split
    String::rsplit(Request, StartPos, " ", BasePropsFound);

    DBG(180, "HTTP Version:" << BasePropsFound.at(0) << " File:" << BasePropsFound.at(1) << " Method:" << BasePropsFound.at(2));
    DBG(180, "HTTP Message (c_str):" << Request.c_str());

    uint8_t HTTPVersion = BasePropsFound.at(0) == "HTTP/1.2" ? 2 : 1;
    size_t PythonReqFound = BasePropsFound.at(1).find("/python/");

    if (PythonReqFound != BasePropsFound.at(1).npos) {
        //- get Host header
        BasePropsResult_t BaseProps;
        RequestHeaderResult_t Headers;
        this->_parseBasePropsRV(Request, BaseProps);
        this->_parseHeadersRV(Request, Headers);
        //- append to ASRequestHandler
        ASRequestHandlerRef->addRequest( { Headers.at("Host"), _ClientFD, HTTPVersion, getNextReqNr(), Request } );
        return 1;
    }
    else {
        //- set values in get requests shared memory
        const char* MsgCString = Request.c_str();

        void* HTTPVersionAddr = getCurrentOffsetAddress();
        void* RequestNrAddr = getCurrentOffsetAddress();
        void* ClientFDAddr = getCurrentOffsetAddress();
        void* MsgLengthAddr = getNextAddress();

        new(HTTPVersionAddr) uint8_t(HTTPVersion);
        new(RequestNrAddr) uint16_t(getNextReqNr());
        new(ClientFDAddr) ClientFD_t(_ClientFD);

        uint16_t MsgLength = Request.length();
        new(MsgLengthAddr) uint16_t(MsgLength);

        void* MsgAddress = getNextAddress();
        memcpy(MsgAddress, &MsgCString[0], MsgLength);

        void* NextSegmentAddr = getNextAddress(MsgLength);
        DBG(120, "Set SharedMem ClientFD:" << ClientFDAddr << " MsgLength:" << MsgLengthAddr << " Msg:" << MsgAddress << " NextSegment:" << NextSegmentAddr);
        return 0;
    }
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
    vector<std::string> Lines;
    String::split(Request, "\n", Lines);

    //- loop over lines, split, put into result map
    for (auto Line:Lines) {

        DBG(120, "Line:'" << Line << "'");

        vector<std::string> HeaderPair;
        String::rsplit(Line, Line.length(), ": ", HeaderPair);

        std::string HeaderID = HeaderPair.at(1);
        std::string HeaderValue = HeaderPair.at(0).substr(0, HeaderPair.at(0).length()-1);

        DBG(120, "HeaderID:'" << HeaderID << "'");
        DBG(120, "HeaderValue:'" << HeaderValue << "'");

        ResultRef.insert(
            HeaderPair_t(HeaderID, HeaderValue)
        );
    }
}
