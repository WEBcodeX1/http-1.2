#include "httpparser.hpp"

using namespace std;


HTTPParser::HTTPParser(const uint16_t BufferSize) :
    _RequestCount(0),
    _RequestCountGet(0),
    _RequestCountPost(0),
    _RequestParseError(0),
    _HTTPRequestBuffer("")
{
    _HTTPRequestBuffer.reserve(BufferSize);
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
    const size_t EndMarkerFound = _HTTPRequestBuffer.find(HTTP_1_1_END_MARKER);

    if (EndMarkerFound != string::npos) {
        _splitRequests();
    }
}

inline void HTTPParser::_splitRequests()
{
    //-> split requests into _SplittedRequests vector
    StringHelper::split(_HTTPRequestBuffer, HTTP_1_1_END_MARKER, _SplittedRequests);

    _RequestCount += _SplittedRequests.size();
}

inline void HTTPParser::processRequests()
{
    //- iterate over splitted requests
    for(size_t i=0; i<_SplittedRequests.size(); ++i) {
        if (_processRequestProperties(i) == false) {
            _RequestParseError = 400;
            break;
        }
    }
}

inline bool HTTPParser::_processRequestProperties(const size_t Index)
{
    //- get request ref at vector index
    auto &Request = _SplittedRequests.at(Index);

    //- on empty request return
    if (Request.empty()) { return false; }

    //- init unparsed base properties with default values
    BaseProperties_t BaseProperties = {
        .HTTPVersion = HTTP_VERSION_UNKNOWN,
        .HTTPMethod = HTTP_METHOD_OTHER,
        .URL = "/"
    };

    //- parse base properties
    _parseRequestProperties(Request, BaseProperties);

    //- only process HTTP/1.1 requests
    if (BaseProperties.HTTPVersion != HTTP_VERSION_1_1) { return false; }

    //- if not GET || POST method, return
    if (BaseProperties.HTTPMethod == HTTP_METHOD_OTHER) { return false; }

    RequestHeader_t Headers;

    //- parse request headers
    _parseRequestHeaders(Request, Headers);

    //- GET request
    if (BaseProperties.HTTPMethod == HTTP_METHOD_GET) {
        //- parse GET parameters into vector
    }

    //- AS POST request
    if (BaseProperties.HTTPMethod == HTTP_METHOD_POST) {

        //- if no content-length header
        if (Headers.find(HTTP_HEADER_CONTENT_LENGTH) != Headers.end()) {
            return false;
        }

        //- if content-length header contains non-digits
        if (StringHelper::is_digits(Headers.at(HTTP_HEADER_CONTENT_LENGTH)) == false) {
            return false;
        }

        uint16_t ContentBytes = stoi(Headers.at(HTTP_HEADER_CONTENT_LENGTH));

        string Payload = "";

        //- if next splitted message exists
        if (_SplittedRequests.size() > Index) {
            auto &NextRequest = _SplittedRequests.at(Index+1);
            if (NextRequest.length() >= ContentBytes) {
                Payload = NextRequest.substr(0, ContentBytes);
                NextRequest.replace(0, ContentBytes, "");
            }
            else {
                return false;
            }
        }
        else {
            //- try payload in _HTTPRequestBuffer
            if (_HTTPRequestBuffer.length() >= ContentBytes) {
                Payload = _HTTPRequestBuffer.substr(0, ContentBytes);
                _HTTPRequestBuffer.replace(0, ContentBytes, "");
            }
        }
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

void HTTPParser::_parseRequestHeaders(string& Request, RequestHeaderRef_t ResultRef)
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
