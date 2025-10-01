#include "httpparser.hpp"

using namespace std;


HTTPParser::HTTPParser(ClientFD_t ClientFD, NamespacesRef_t NamespacesRef) :
    Client(ClientFD),
    _Namespaces(NamespacesRef),
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
    //-> reset _SplittedRequests vector
    _SplittedRequests.clear();

    //- workaround
    if (BufferRef[0] == 0) { return; }

    DBG(250, "size:" << string(BufferRef).length() << "appendBuffer:'" << string(BufferRef) << "'");
    _HTTPRequestBuffer = _HTTPRequestBuffer + string(BufferRef);
    //String::hexout(_HTTPRequestBuffer);
    DBG(250, "_HTTPRequestBuffer:'" << _HTTPRequestBuffer << "'");

    //-> only process on min 1 valid request
    const size_t EndMarkerFound = _HTTPRequestBuffer.find("\r\n\r\n");

    if (EndMarkerFound != string::npos) {
        _splitRequests();
    }
}

inline void HTTPParser::_splitRequests()
{
    //DBG(180, "splitRequests Buffer:'" << _HTTPRequestBuffer << "'");

    //- reset request counters
    _RequestCountGet = 0;
    _RequestCountPost = 0;
    _RequestCountPostAS = 0;
    _RequestNumber = 1;

    //-> split requests into _SplittedRequests vector
    String::split(_HTTPRequestBuffer, "\r\n\r\n", _SplittedRequests);

    _RequestCount = _SplittedRequests.size();
    DBG(120, "splitRequests count after splitted into Vector:" << _RequestCount);
    DBG(120, "_HTTPRequestBuffer after split:'" << _HTTPRequestBuffer << "'");
}

size_t HTTPParser::processRequests(SharedMemAddress_t SHMGetRequests, const ASRequestHandlerRef_t& ASRequestHandlerRef)
{
    DBG(250, "_HTTPRequestBuffer:'" << _HTTPRequestBuffer << "'");

    //- set get requests SHM base
    setBaseAddress(SHMGetRequests);

    //- iterate over splitted requests
    for(size_t i=0; i<_SplittedRequests.size(); ++i) {
        _processRequestProperties(i, ASRequestHandlerRef);
    }

    return _RequestCountGet;
}

void HTTPParser::_processRequestProperties(const size_t Index, const ASRequestHandlerRef_t& ASRequestHandlerRef)
{
    //- get request ref at vector index
    auto &Request = _SplittedRequests.at(Index);

    //- on empty request return
    if (Request.empty()) { return; }

    DBG(140, "Processing Index:" << Index << " Request:'" << Request << "'");

    BasePropsResult_t BasePropsFound;
    _parseRequestProperties(Request, BasePropsFound);

    DBG(140, "HTTP Version:" << BasePropsFound.at(0) << " File:" << BasePropsFound.at(1) << " Method:" << BasePropsFound.at(2));
    DBG(140, "Complete Request:" << Request.c_str());

    //- temp hardcode HTTPVersion
    uint16_t HTTPVersion = 1;

    //- check HTTP/1.2 or HTTP/1.2 (currently unimplemented)
    const size_t HTTPVersion1_1Found = BasePropsFound.at(0).find("HTTP/1.1");
    const size_t HTTPVersion1_2Found = BasePropsFound.at(2).find("HTTP/1.2");

    //- if not HTTP/1.1 set request to "" in vector element, return
    if (HTTPVersion1_1Found == string::npos) { return; }

    //- check for method GET || POST
    const size_t HTTPMethodPOST = BasePropsFound.at(2).find("POST");
    const size_t HTTPMethodGET = BasePropsFound.at(2).find("GET");

    //- set numerical http method (GET: 1, POST: 2)
    uint16_t HTTPMethod = 0;

    if (HTTPMethodGET != string::npos) { HTTPMethod = 1; }
    if (HTTPMethodPOST != string::npos) { HTTPMethod = 2; }

    DBG(140, "HTTPMethod:" << HTTPMethod);

    //- if not GET || POST, return
    if (HTTPMethod == 0) { return; }

    //- check if POST request is a POSTAS request
    const size_t AppServerReqFound = BasePropsFound.at(1).find("/backend/");

    //- if not POSTAS, return
    if (HTTPMethod == 2 && AppServerReqFound == string::npos) {
        ++_RequestCountPost;
        return;
    }

    //- get unique request nr
    const uint16_t RequestNr = _RequestNumber;
    ++_RequestNumber;

    DBG(140, "HTTP RequestNr:" << RequestNr);

    RequestHeaderResult_t Headers;

    //- AS GET request
    if (HTTPMethod == 1 && AppServerReqFound != string::npos) {

        //- parse request headers
        _parseRequestHeaders(Request, Headers);

        const NamespaceProps_t NamespaceProps = _Namespaces.at(Headers.at("Host"));
        string JSONPayload("{ \"payload\": {");

        for (const auto& [Endpoint, EndpointProps]: NamespaceProps.JSONConfig["access"]["as-get"].items()) {
            DBG(200, "Endpoint:" << Endpoint);
            const size_t EndpointFound = BasePropsFound.at(1).find("/backend" + Endpoint);
            if (EndpointFound != string::npos) {
                DBG(200, "Looping on params");
                string ProcessURL = BasePropsFound.at(1);
                for (size_t i=0; i<EndpointProps["params"].size(); ++i) {
                    const string Param = EndpointProps["params"][i];
                    const string ParamValue = _getASURLParamValue(Param, i, ProcessURL);
                    JSONPayload += "\"" + Param + "\": \"" + ParamValue + "\"";
                    DBG(200, "ProcessURL: " << ProcessURL << " Param: " << Param << " Value:" << ParamValue);
                    if (i != EndpointProps["params"].size() - 1) {
                        JSONPayload += ",";
                    }
                }
                JSONPayload += "}}";
                DBG(200, "AS GET JSONPayload:" << JSONPayload);
                _processASPayload(
                    ASRequestHandlerRef, Headers, HTTPMethod, HTTPVersion, RequestNr, JSONPayload
                );
                return;
            }
        }
    }

    //- AS POST request
    if (HTTPMethod == 2 && AppServerReqFound != string::npos) {

        DBG(140, "Request Type ASRequest:" << Request);

        //- check first line end exists
        size_t FirstLineEndMarker = Request.find("\r\n");

        //- if not truncated
        if (FirstLineEndMarker == string::npos) {
            DBG(200, "Truncated POST Request - no First Line end");
            return;
        }

        //- cut first properties line from request
        if (FirstLineEndMarker != string::npos) {
            Request.replace(0, FirstLineEndMarker+2, "");
        }

        //- parse request headers
        _parseRequestHeaders(Request, Headers);

        uint ContentBytes = 0;

        //- try get content length header
        try {
            ContentBytes = stoi(Headers.at("Content-Length"));
        }
        catch(const std::exception& e) {
            DBG(200, "Truncated POST Request - no Content-Length");
            return;
        }

        bool PayloadFound = false;
        string Payload = "";

        //- try payload in next (vector index +1) request
        try {
            auto &NextRequest = _SplittedRequests.at(Index+1);
            if (NextRequest.length() >= ContentBytes) {
                Payload = NextRequest.substr(0, ContentBytes);
                NextRequest.replace(0, ContentBytes, "");
                PayloadFound = true;
            }
        }
        catch(const std::exception& e) {
            DBG(200, "Next vector does not exist, trying in rest of _HTTPRequestBuffer");
            //- try payload in _HTTPRequestBuffer
            if (_HTTPRequestBuffer.length() >= ContentBytes) {
                Payload = _HTTPRequestBuffer.substr(0, ContentBytes);
                _HTTPRequestBuffer.replace(0, ContentBytes, "");
                PayloadFound = true;
            }
        }

        DBG(140, "HTTP POST-AS Payload:" << Payload);

        if (PayloadFound) {
            _processASPayload(
                ASRequestHandlerRef, Headers, HTTPMethod, HTTPVersion, RequestNr, Payload
            );
        }
    }

    //- Standard GET request
    if (HTTPMethod == 1 && AppServerReqFound == string::npos) {

        DBG(140, "Request Type GET:" << Request);

        ++_RequestCountGet;

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

            ResultRef.emplace(
                HeaderID, HeaderValue
            );
        }
    }
    DBG(120, "End parse headers.");
}

inline string HTTPParser::_getASURLParamValue(
    const string& Param,
    const uint16_t Index,
    string& ReqURL
){
    DBG(200, "ReqURL:" << ReqURL);
    //- process first ? parameter
    if (Index == 0) {
        const size_t StartMarkerPos = ReqURL.find("?");
        const size_t MidMarkerPos = ReqURL.find("=");
        const size_t EndMarkerPos = ReqURL.find("&");
        const size_t CompletePos = ReqURL.find("?" + Param + "=");
        const size_t CheckMidPos = CompletePos+Param.size()+1;

        if (CompletePos != string::npos && StartMarkerPos != string::npos && MidMarkerPos != string::npos && MidMarkerPos == CheckMidPos) {
            const size_t EndPos = (EndMarkerPos == string::npos) ? ReqURL.size() : EndMarkerPos;
            const string ReturnString = ReqURL.substr(MidMarkerPos+1, EndPos-(MidMarkerPos+1));
            DBG(200, "ReqURL StartMarkerPos:" << StartMarkerPos << " EndPos:" << EndPos);
            ReqURL.replace(StartMarkerPos, EndPos-StartMarkerPos, "");
            return ReturnString;
        }
        else {
            return "not-found";
        }
    }
    //- process next & parameter(s)
    if (Index > 0) {
        const size_t StartMarkerPos = ReqURL.find("&");
        const size_t MidMarkerPos = ReqURL.find("=");
        const size_t CompletePos = ReqURL.find("&" + Param + "=");
        const size_t CheckMidPos = CompletePos+Param.size()+1;

        if (CompletePos != string::npos && StartMarkerPos != string::npos && MidMarkerPos != string::npos && MidMarkerPos == CheckMidPos) {
            const size_t NextMarkerPos = ReqURL.find("&", MidMarkerPos);
            const size_t EndPos = (NextMarkerPos == string::npos) ? ReqURL.size() : NextMarkerPos;
            const string ReturnString = ReqURL.substr(MidMarkerPos+1, EndPos-(MidMarkerPos+1));
            DBG(200, "ReqURL StartMarkerPos:" << StartMarkerPos << " EndPos:" << EndPos);
            ReqURL.replace(StartMarkerPos, EndPos-StartMarkerPos, "");
            return ReturnString;
        }
        else {
            return "not-found";
        }
    }
    return "parse-error";
}

inline void HTTPParser::_processASPayload(
    const ASRequestHandlerRef_t& ASRequestHandlerRef,
    const RequestHeaderResult_t& Headers,
    const uint16_t HTTPMethod,
    const uint16_t HTTPVersion,
    const uint16_t RequestNr,
    const string& Payload
){
    //- increment request count
    ++_RequestCountPostAS;

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
