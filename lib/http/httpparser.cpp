#include "httpparser.hpp"

using namespace std;


HTTPParser::HTTPParser(ClientFD_t ClientFD) :
    Client(ClientFD),
    _RequestCount(0),
    _RequestCountGet(0),
    _RequestCountPost(0),
    _RequestCountPostAS(0),
    _HTTPRequestBuffer("")
{
    DBG(120, "Constructor");
}

HTTPParser::~HTTPParser()
{
    DBG(120, "Destructor");
}

void HTTPParser::appendBuffer(const char* BufferRef, const uint16_t BufferSize)
{
    _HTTPRequestBuffer = _HTTPRequestBuffer + string(&BufferRef[0], BufferSize);
    //String::hexout(_HTTPRequestBuffer);
    DBG(250, "_HTTPRequestBuffer:'" << _HTTPRequestBuffer << "'");

    //-> only process on min 1 valid request
    const size_t EndMarkerFound = _HTTPRequestBuffer.find("\r\n\r\n");

    if (EndMarkerFound != string::npos) {
        _splitRequests();
    }
}

void HTTPParser::_splitRequests()
{
    //DBG(180, "splitRequests Buffer:'" << _HTTPRequestBuffer << "'");

    //- reset request counters
    _RequestCountGet = 0;
    _RequestCountPost = 0;
    _RequestCountPostAS = 0;

    //-> reset _SplittedRequests vector
    _SplittedRequests.clear();

    //-> split requests into _SplittedRequests vector
    String::split(_HTTPRequestBuffer, "\r\n\r\n", _SplittedRequests);
    _RequestCount = _SplittedRequests.size();
    DBG(120, "splitRequests count after splitted into Vector:" << _RequestCount);
}

size_t HTTPParser::processRequests(SharedMemAddress_t SHMGetRequests, const ASRequestHandlerRef_t ASRequestHandlerRef)
{
    //- set get requests SHM base
    setBaseAddress(SHMGetRequests);

    //- iterate over splitted requests
    for(size_t i=0; i<_SplittedRequests.size(); ++i) {
        _processRequestProperties(i, ASRequestHandlerRef);
    }

    //- restore truncated requests
    const auto LastElementIndex = _SplittedRequests.size();

    if (LastElementIndex > 0) {
        const auto LastElementVal = _SplittedRequests.at(LastElementIndex-1);

        if (!LastElementVal.empty()) {
            _HTTPRequestBuffer.append(LastElementVal);
        }
    }

    if (LastElementIndex > 1) {
        const auto LastElementPrevVal = _SplittedRequests.at(LastElementIndex-2);

        if (!LastElementPrevVal.empty()) {
            _HTTPRequestBuffer.append(LastElementPrevVal);
        }
    }

    return _RequestCountGet;
}

void HTTPParser::_processRequestProperties(const size_t Index, const ASRequestHandlerRef_t ASRequestHandlerRef)
{
    DBG(140, "Processing Index:" << Index);

    //- get request ref at vector index
    auto Request = _SplittedRequests.at(Index);

    //- on empty request return
    if (Request.empty()) { return; }

    BasePropsResult_t BasePropsFound;
    this->_parseRequestProperties(Request, BasePropsFound);

    DBG(140, "HTTP Version:" << BasePropsFound.at(0) << " File:" << BasePropsFound.at(1) << " Method:" << BasePropsFound.at(2));
    DBG(140, "Complete Request:" << Request.c_str());

    //- temp hardcode HTTPVersion
    uint16_t HTTPVersion = 1;

    //- check HTTP/1.2 or HTTP/1.2 (currently unimplemented)
    const size_t HTTPVersion1_1Found = BasePropsFound.at(0).find("HTTP/1.1");
    const size_t HTTPVersion1_2Found = BasePropsFound.at(2).find("HTTP/1.2");

    //- if not HTTP/1.1 set request to "" in vector element, return
    if (HTTPVersion1_1Found == string::npos) {
        _SplittedRequests.at(Index) = "";
        return;
    }

    //- check for method GET || POST
    const size_t HTTPMethodPOST = BasePropsFound.at(2).find("POST");
    const size_t HTTPMethodGET = BasePropsFound.at(2).find("GET");

    //- set numerical http method (GET: 1, POST: 2)
    uint16_t HTTPMethod = 0;

    if (HTTPMethodGET != string::npos) { HTTPMethod = 1; }
    if (HTTPMethodPOST != string::npos) { HTTPMethod = 2; }

    DBG(140, "HTTPMethod:" << HTTPMethod);

    //- if not GET || POST, set request to "" in vector element, return
    if (HTTPMethod == 0) {
        _SplittedRequests.at(Index) = "";
        return;
    }

    //- check if POST request is a POSTAS request
    const size_t PythonReqFound = BasePropsFound.at(1).find("/python/");

    //- if not POSTAS, set request to "" in vector element, return
    if (HTTPMethod == 2 && PythonReqFound == string::npos) {
        ++this->_RequestCountPost;
        _SplittedRequests.at(Index) = "";
        return;
    }

    //- get unique request nr
    const uint16_t RequestNr = getNextReqNr();

    DBG(140, "HTTP RequestNr:" << RequestNr);

    if (HTTPMethod == 2 && PythonReqFound != string::npos) {

        DBG(140, "Request Type PythonAS:" << Request);

        //-> check first line end exists
        size_t FirstLineEndMarker = Request.find("\r\n");

        //-> if not: truncated
        if (FirstLineEndMarker == string::npos) {
            DBG(200, "Truncated POST Request - no First Line end");
            return;
        }

        //-> cut first properties line from request
        if (FirstLineEndMarker != string::npos) {
            Request.replace(0, FirstLineEndMarker+2, "");
        }

        RequestHeaderResult_t Headers;
        this->_parseRequestHeaders(Request, Headers);

        uint ContentBytes = 0;

        //- try get content length header
        try {
            ContentBytes = stoi(Headers.at("Content-Length"));
        }
        catch(const std::exception& e) {
            DBG(200, "Truncated POST Request - no Content-Length");
            return;
        }

        //- try get next request +payload at vector index +1
        try {
            auto &NextRequest = _SplittedRequests.at(Index+1);

            if (NextRequest.length() >= ContentBytes) {
                string Payload = NextRequest.substr(0, ContentBytes);
                NextRequest.replace(0, ContentBytes, "");
                DBG(140, "HTTP POST-AS Payload:" << Payload << " NextRequest:'" << NextRequest << "'");

                //- increment request count
                ++this->_RequestCountPostAS;

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
        }
        catch(const std::exception& e) {
            DBG(200, "Truncated POST Request - no Payload");
            return;
        }
    }

    if (HTTPMethod == 1) {

        DBG(140, "Request Type GET:" << Request);

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

    //-> if no headers (no \r\n), set start pos to end of string
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

    DBG(120, "Last Header Line:'" << Request << "'");
    Lines.push_back(Request);

    //- loop over lines, split, put into result map
    for (auto &Line:Lines) {

        DBG(120, "Line:'" << Line << "'");

        vector<string> HeaderPair;
        if (Line.find(":") != string::npos) {

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
