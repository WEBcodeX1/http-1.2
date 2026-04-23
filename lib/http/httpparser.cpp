#include "httpparser.hpp"

using namespace std;


HTTPParser::HTTPParser() :
    _RequestCount(0),
    _RequestCountGet(0),
    _RequestCountPost(0),
    _HTTPRequestBuffer("")
{
    _HTTPRequestBuffer.reserve(BUFFER_BYTES);
}

HTTPParser::~HTTPParser()
{
}

void HTTPParser::appendBuffer(const char* BufferRef, const uint16_t RecvBytes)
{
    //-> reset _SplittedRequests vector
    _SplittedRequests.clear();

    _HTTPRequestBuffer.append(BufferRef, RecvBytes);

    //-> only process on minimum of 1 http request (end request marker found)
    const size_t EndMarkerFound = _HTTPRequestBuffer.find("\r\n\r\n");

    if (EndMarkerFound != string::npos) {
        _splitRequests();
    }
}

inline void HTTPParser::_splitRequests()
{
    //- reset request counters
    _RequestCountGet = 0;
    _RequestCountPost = 0;
    _RequestCountPostAS = 0;
    _RequestNumber = 1;

    //-> split requests into _SplittedRequests vector
    StringHelper::split(_HTTPRequestBuffer, "\r\n\r\n", _SplittedRequests);

    _RequestCount = _SplittedRequests.size();
}

size_t HTTPParser::processRequests()
{
    //- iterate over splitted requests
    for(size_t i=0; i<_SplittedRequests.size(); ++i) {
        _processRequestProperties(i);
    }

    return _RequestCountGet;
}

void HTTPParser::_processRequestProperties(const size_t Index)
{
    //- get request ref at vector index
    auto &Request = _SplittedRequests.at(Index);

    //- on empty request return
    if (Request.empty()) { return; }

    //- init unparsed base properties with default values
    BaseProperties_t BaseProperties = {
        .HTTPVersion = HTTP_VERSION_UNKNOWN,
        .HTTPMethod = HTTP_METHOD_OTHER,
        .URL = "/"
    };

    //- parse base properties
    _parseRequestProperties(Request, BaseProperties);

    //- only process HTTP/1.1 requests
    if (BaseProperties.HTTPVersion != HTTP_VERSION_1_1) { return; }

    //- if not GET || POST method, return
    if (BaseProperties.HTTPMethod == HTTP_METHOD_OTHER) { return; }



    RequestHeaderResult_t Headers;

    //- AS GET request
    if (HTTPMethod == 1 && AppServerReqFound != string::npos) {

        //- parse request headers
        _parseRequestHeaders(Request, Headers);

        /*
        const NamespaceProps_t NamespaceProps = _Namespaces.at(Headers.at("Host"));
        string JSONPayload("{ \"payload\": {");

        for (const auto& [Endpoint, EndpointProps]: NamespaceProps.JSONConfig["access"]["as-get"].items()) {
            DBG(200, "Endpoint:" << Endpoint);
            const size_t EndpointFound = BaseProperties.at(1).find("/backend" + Endpoint);
            if (EndpointFound != string::npos) {
                DBG(200, "Looping on params");
                string ProcessURL = BaseProperties.at(1);
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
        */
    }

    //- AS POST request
    if (HTTPMethod == 2 && AppServerReqFound != string::npos) {

        //- check first line end exists
        size_t FirstLineEndMarker = Request.find("\r\n");

        //- if not truncated
        if (FirstLineEndMarker == string::npos) {
            return;
        }

        //- cut first properties line from request
        if (FirstLineEndMarker != string::npos) {
            Request.replace(0, FirstLineEndMarker+2, "");
        }

        //- parse request headers
        _parseRequestHeaders(Request, Headers);

        uint16_t ContentBytes = 0;

        //- try get content length header
        try {
            ContentBytes = stoi(Headers.at("Content-Length"));
        }
        catch(const std::exception& e) {
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
            //- try payload in _HTTPRequestBuffer
            if (_HTTPRequestBuffer.length() >= ContentBytes) {
                Payload = _HTTPRequestBuffer.substr(0, ContentBytes);
                _HTTPRequestBuffer.replace(0, ContentBytes, "");
                PayloadFound = true;
            }
        }

        /*
        if (PayloadFound) {
            _processASPayload(
                ASRequestHandlerRef, Headers, HTTPMethod, HTTPVersion, RequestNr, Payload
            );
        }
        */
    }

    //- Standard GET request
    if (HTTPMethod == 1 && AppServerReqFound == string::npos) {

        ++_RequestCountGet;

        /*
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
        */
    }
}

void HTTPParser::_parseRequestProperties(string& Request, BasePropertiesRef_t ResultBaseProps)
{
    //- find first line endline
    size_t StartPos = Request.find("\r\n");

    //-> if no headers (no \r\n), set start pos to end of string
    if (StartPos == string::npos) {
        StartPos = Request.length();
    }

    //- get base request properties
    vector<string> SplitResult;
    StringHelper::rsplit(Request, StartPos, " ", SplitResult);

    if (SplitResult.at(0).find("HTTP/1.1") != string::npos) {
        ResultBaseProps.HTTPVersion = HTTP_VERSION_1_1;
    }

    if (SplitResult.at(2).find("GET") != string::npos) {
        ResultBaseProps.HTTPMethod = HTTP_METHOD_GET;
    }
    else if (SplitResult.at(2).find("POST") != string::npos) {
        ResultBaseProps.HTTPMethod = HTTP_METHOD_POST;
    }

    ResultBaseProps.URL = SplitResult.at(1);
}

void HTTPParser::_parseRequestHeaders(string& Request, RequestHeaderResultRef_t ResultRef)
{
    //- reverse split header lines
    vector<string> Lines;
    StringHelper::split(Request, "\r\n", Lines);

    Lines.push_back(Request);

    //- loop over lines, split, put into result map
    for (auto &Line:Lines) {

        vector<string> HeaderPair;
        if (Line.find(":") != string::npos) {

            StringHelper::rsplit(Line, Line.length(), ": ", HeaderPair);
            string HeaderID = HeaderPair.at(1);
            string HeaderValue = HeaderPair.at(0).substr(0, HeaderPair.at(0).length());

            ResultRef.emplace(
                HeaderID, HeaderValue
            );
        }
    }
}

inline uint16_t HTTPParser::_getURLParamValue(
    const string& Param,
    const uint16_t Index,
    string& ReqURL
){
    //- process first "?" parameter
    if (Index == 0) {
        const size_t StartMarkerPos = ReqURL.find("?");
        const size_t MidMarkerPos = ReqURL.find("=");
        const size_t EndMarkerPos = ReqURL.find("&");
        const size_t CompletePos = ReqURL.find("?" + Param + "=");
        const size_t CheckMidPos = CompletePos+Param.size()+1;

        if (CompletePos != string::npos && StartMarkerPos != string::npos && MidMarkerPos != string::npos && MidMarkerPos == CheckMidPos) {
            const size_t EndPos = (EndMarkerPos == string::npos) ? ReqURL.size() : EndMarkerPos;
            const string ReturnString = ReqURL.substr(MidMarkerPos+1, EndPos-(MidMarkerPos+1));
            ReqURL.replace(StartMarkerPos, EndPos-StartMarkerPos, "");
            return ReturnString;
        }
        else {
            return URL_PARAM_NOT_FOUND;
        }
    }

    //- process next "&" parameter(s)
    if (Index > 0) {
        const size_t StartMarkerPos = ReqURL.find("&");
        const size_t MidMarkerPos = ReqURL.find("=");
        const size_t CompletePos = ReqURL.find("&" + Param + "=");
        const size_t CheckMidPos = CompletePos+Param.size()+1;

        if (CompletePos != string::npos && StartMarkerPos != string::npos && MidMarkerPos != string::npos && MidMarkerPos == CheckMidPos) {
            const size_t NextMarkerPos = ReqURL.find("&", MidMarkerPos);
            const size_t EndPos = (NextMarkerPos == string::npos) ? ReqURL.size() : NextMarkerPos;
            const string ReturnString = ReqURL.substr(MidMarkerPos+1, EndPos-(MidMarkerPos+1));
            ReqURL.replace(StartMarkerPos, EndPos-StartMarkerPos, "");
            return ReturnString;
        }
        else {
            return URL_PARAM_NOT_FOUND;
        }
    }
    return URL_PARAM_PARSE_ERROR ;
}
