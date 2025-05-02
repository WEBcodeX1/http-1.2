#include "httpparser.hpp"

using namespace std;


HTTPParser::HTTPParser(ClientFD_t ClientFD) :
    Client(ClientFD),
    _RequestCount(0),
    _RequestCountGet(0),
    _RequestCountPost(0),
    _RequestCountPostAS(0),
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
    String::hexout(_HTTPRequest);
    //DBG(250, "HTTPRequest:'" << _HTTPRequest << "'");
    _splitRequests();
}

void HTTPParser::_splitRequests()
{
    //DBG(180, "splitRequests Buffer:'" << _HTTPRequest << "'");

    //- reset request counters
    _RequestCountGet = 0;
    _RequestCountPost = 0;
    _RequestCountPostAS = 0;

    //-> reset incomplete request string
    string incompleteRequest("");

    //-> check for existing valid request end marker
    size_t LastDelimiterPos = _HTTPRequest.rfind("\r\n\r\n");
    DBG(120, "LastDelimiterPos:" << LastDelimiterPos);

    //-> if min 1 full valid request && rest without end marker
    if (LastDelimiterPos != string::npos && LastDelimiterPos != _HTTPRequest.length()-4) {

        //-> put incomplete last request into tmp string
        incompleteRequest = _HTTPRequest.substr(LastDelimiterPos+4);

        //-> remove incomplete last request from _HTTPRequest
        _HTTPRequest.replace(_HTTPRequest.begin()+LastDelimiterPos+4, _HTTPRequest.end(), "");
    }

    //-> split requests into _SplittedRequests vector
    _SplittedRequests.clear();
    String::split(_HTTPRequest, "\r\n\r\n", _SplittedRequests);
    _RequestCount = _SplittedRequests.size();
    DBG(120, "splitRequests count after splitted into Vector:" << _RequestCount);

    //-> "restore" incomplete last request buffer
    _HTTPRequest = incompleteRequest;
}

uint HTTPParser::processRequests(SharedMemAddress_t SHMGetRequests, const ASRequestHandlerRef_t ASRequestHandlerRef)
{
    setBaseAddress(SHMGetRequests);

    for (auto &Request:_SplittedRequests) {
        _processRequestProperties(Request, ASRequestHandlerRef);
    }

    return _RequestCountGet;
}

void HTTPParser::_processRequestProperties(string& Request, const ASRequestHandlerRef_t ASRequestHandlerRef)
{
    //DBG(180, "HTTP Request:'" << Request << "'");

    BasePropsResult_t BasePropsFound;
    this->_parseRequestProperties(Request, BasePropsFound);

    DBG(140, "HTTP Version:" << BasePropsFound.at(0) << " File:" << BasePropsFound.at(1) << " Method:" << BasePropsFound.at(2));
    DBG(140, "HTTP Payload (c_str):" << Request.c_str());

    //- check HTTP/1.2 (currently unimplemented)
    const size_t HTTPVersion1_2Found = BasePropsFound.at(0).find("HTTP/1.2");

    //- if not HTTP/1.1, do not process further
    const size_t HTTPVersion1_1Found = BasePropsFound.at(0).find("HTTP/1.1");

    const uint16_t HTTPMethod = (BasePropsFound.at(2).find("POST") != string::npos) ? 2 : 1;
    const uint16_t HTTPVersion = (BasePropsFound.at(0).find("HTTP/1.1") != string::npos) ? 1 : 2;

    //- check if POST request is an AS request
    const size_t PythonReqFound = BasePropsFound.at(1).find("/python/");

    //- get unique request nr
    uint16_t RequestNr = getNextReqNr();

    DBG(140, "HTTP RequestNr:" << RequestNr << " HTTPVersion:" << HTTPVersion << " HTTPMethod:" << HTTPMethod);

    if (HTTPMethod == 2) {
        ++this->_RequestCountPost;
    }

    if (HTTPMethod == 2 && PythonReqFound != BasePropsFound.at(1).npos) {

        DBG(140, "Python Request:" << Request);

        ++this->_RequestCountPostAS;

        //-> cut first properties line from request
        size_t FirstLineEndMarker = Request.find("\n\r");
        Request.replace(0, FirstLineEndMarker+2, "");

        RequestHeaderResult_t Headers;
        this->_parseRequestHeaders(Request, Headers);

        auto ContentBytes = stoi(Headers.at("Content-Length"));
        string Payload = Request.substr(Request.length()-ContentBytes, ContentBytes);

        DBG(140, "HTTP POST-AS payload:" << Payload);

        //- add ASRequestHandler request
        ASRequestHandlerRef->addRequest({
            Headers.at("Host"),
            _ClientFD,
            HTTPMethod,
            HTTPVersion,
            RequestNr,
            Payload
        });
    }

    if (HTTPMethod == 1) {

        ++this->_RequestCountGet;

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
    }
}

void HTTPParser::_parseRequestProperties(string& Request, BasePropsResultRef_t ResultRef)
{
    DBG(120, "HTTP Request:'" << Request << "'");

    //- find first line endline
    size_t StartPos = Request.find("\r\n");

    //-> if no headers (no \n), set start pos to end of string
    if (StartPos == string::npos) {
        StartPos = Request.length();
    }

    //- reverse split
    String::rsplit(Request, StartPos, " ", ResultRef);

    DBG(120, "HTTP Version:" << ResultRef.at(0) << " File:" << ResultRef.at(1) << " Method:" << ResultRef.at(2) << " Request:" << Request);
}

void HTTPParser::_parseRequestHeaders(string& Request, RequestHeaderResultRef_t ResultRef)
{
    DBG(120, "HTTP Request:'" << Request << "'");

    //- reverse split header lines
    vector<string> Lines;
    String::split(Request, "\r\n", Lines);

    //- loop over lines, split, put into result map
    for (auto &Line:Lines) {

        DBG(120, "Line:'" << Line << "'");

        vector<string> HeaderPair;
        if (Line.find(':') != string::npos) {

            String::rsplit(Line, Line.length(), ": ", HeaderPair);
            string HeaderID = HeaderPair.at(1);
            string HeaderValue = HeaderPair.at(0).substr(0, HeaderPair.at(0).length());

            DBG(120, "HeaderID:'" << HeaderID << "'");
            DBG(120, "HeaderValue:'" << HeaderValue << "'");

            ResultRef.insert(
                HeaderPair_t(HeaderID, HeaderValue)
            );
        }
    }
    DBG(120, "End parse headers.");
}
