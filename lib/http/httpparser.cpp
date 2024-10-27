#include "httpparser.hpp"

using namespace std;


HTTPParser::HTTPParser(ClientFD_t ClientFD) :
    Client(ClientFD),
    _RequestCount(0),
    _HTTPRequest(""),
    _RegexContentLength(regex(".*Content-Length: ([0-9]+).*", regex::extended))
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
    DBG(250, "Buffer:'" << _HTTPRequest << "'");
    _splitRequests();
}

void HTTPParser::_splitRequests()
{
    DBG(200, "splitRequests Buffer:'" << _HTTPRequest << "'");

    _SplittedRequests.clear();

    size_t ContentLengthFoundPos = _HTTPRequest.find("Content-Length");
    size_t EndMarkerFoundPos = _HTTPRequest.find("\n\r");

    cmatch RegexMatch;

    uint SplitPosition = 0;

    while(
        (ContentLengthFoundPos < EndMarkerFoundPos) ||
        (ContentLengthFoundPos == string::npos && EndMarkerFoundPos != string::npos)
    )
    {
        if (ContentLengthFoundPos < EndMarkerFoundPos) {
            if (regex_match(_HTTPRequest.c_str(), RegexMatch, _RegexContentLength)) {
                DBG(140, "Content-Length header found.");
                int ContentLength = stoi(RegexMatch[1]);
                SplitPosition = EndMarkerFoundPos+ContentLength+3;
            }
        }
        else if (ContentLengthFoundPos == string::npos && EndMarkerFoundPos != string::npos)
        {
            SplitPosition = EndMarkerFoundPos+3;
        }

        _SplittedRequests.push_back(
            _HTTPRequest.substr(0, SplitPosition)
        );

        _HTTPRequest.erase(0, SplitPosition);

        ContentLengthFoundPos = _HTTPRequest.find("Content-Length");
        EndMarkerFoundPos = _HTTPRequest.find("\n\r");
    }

    _RequestCount = _SplittedRequests.size();
    DBG(120, "splitRequests after split Vector Element count:" << _RequestCount);
}

void HTTPParser::parseRequestsComplete()
{
    for (auto &Request:_SplittedRequests) {
        vector<std::string> RequestLines;
        String::split(Request, "\n", RequestLines);
    }
}

uint HTTPParser::parseRequestsBasic(SharedMemAddress_t SHMGetRequests, const ASRequestHandlerRef_t ASRequestHandlerRef)
{
    setBaseAddress(SHMGetRequests);

    uint16_t PythonRequestCount = 0;

    for (auto &Request:_SplittedRequests) {
        PythonRequestCount += _parseBaseProps(Request, ASRequestHandlerRef);
    }

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

    DBG(140, "HTTP Version:" << BasePropsFound.at(0) << " File:" << BasePropsFound.at(1) << " Method:" << BasePropsFound.at(2));
    DBG(140, "HTTP Payload (c_str):" << Request.c_str());

    uint16_t HTTPMethod = (BasePropsFound.at(2).find("POST") != string::npos) ? 2 : 1;
    uint16_t HTTPVersion = (BasePropsFound.at(0).find("HTTP/1.1") != string::npos) ? 1 : 2;

    size_t PythonReqFound = BasePropsFound.at(1).find("/python/");

    uint16_t RequestNr = getNextReqNr();

    DBG(140, "HTTP RequestNr:" << RequestNr << " HTTPVersion:" << HTTPVersion);

    cmatch RegexMatch;

    if (PythonReqFound != BasePropsFound.at(1).npos) {

        DBG(140, "Python Request:" << Request);

        string Payload = "";

        size_t ContentLengthFoundPos = Request.find("Content-Length");
        size_t EndMarkerFoundPos = Request.find("\n\r");

        if (ContentLengthFoundPos != string::npos && EndMarkerFoundPos != string::npos) {
            if (regex_match(Request.c_str(), RegexMatch, _RegexContentLength)) {
                DBG(140, "Content-Length header found.");
                int ContentLength = stoi(RegexMatch[1]);
                uint SubStrStart = EndMarkerFoundPos+3;
                uint SubStrEnd = EndMarkerFoundPos+ContentLength+3;
                Payload = Request.substr(SubStrStart, SubStrEnd);
            }
        }

        //- get Host header
        BasePropsResult_t BaseProps;
        RequestHeaderResult_t Headers;
        this->_parseBasePropsRV(Request, BaseProps);
        this->_parseHeadersRV(Request, Headers);

        //- add ASRequestHandler request
        ASRequestHandlerRef->addRequest({
            Headers.at("Host"),
            _ClientFD,
            HTTPMethod,
            HTTPVersion,
            RequestNr,
            Payload
        });
        return 1;
    }
    else {
        //- set values in get requests shared memory
        const char* MsgCString = Request.c_str();

        void* ClientFDAddr = getCurrentOffsetAddress();
        void* HTTPVersionAddr = getNextAddress();
        void* RequestNrAddr = getNextAddress();
        void* MsgLengthAddr = getNextAddress();

        new(HTTPVersionAddr) uint16_t(HTTPVersion);
        new(RequestNrAddr) uint16_t(RequestNr);
        new(ClientFDAddr) ClientFD_t(_ClientFD);

        uint16_t MsgLength = Request.length();
        new(MsgLengthAddr) uint16_t(MsgLength);

        void* MsgAddress = getNextAddress();
        memcpy(MsgAddress, &MsgCString[0], MsgLength);

        void* NextSegmentAddr = getNextAddress(MsgLength);
        DBG(120, "Set SharedMem ClientFD:" << ClientFDAddr << " PayloadLength:" << MsgLengthAddr << " Payload:" << MsgAddress << " NextSegment:" << NextSegmentAddr);
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
    for (auto &Line:Lines) {

        if ( Line.find("\n\r") != string::npos ) { break; }

        DBG(120, "Line:'" << Line << "'");

        vector<std::string> HeaderPair;
        if (Line.find(':') != string::npos) {
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
    DBG(120, "End parse headers.");
}
