#include "httpparser.hpp"
#include "httpconstants.hpp"

using namespace std;


HTTPParser::HTTPParser(const uint16_t BufferSize) :
    _RequestCountGet(0),
    _RequestCountPost(0),
    _RequestParseError(0),
    _POSTWaitContentLength(false),
    _POSTContentLength(0),
    _HTTPRequestBuffer("")
{
    _HTTPRequestBuffer.reserve(BufferSize);
    _HTTPRequestBufferMax = BufferSize;
}

HTTPParser::~HTTPParser()
{
}

void HTTPParser::appendBuffer(const char* BufferRef, const uint16_t RecvBytes)
{
    if (_HTTPRequestBuffer.length()+RecvBytes > _HTTPRequestBufferMax) {
        return;
    }

    _HTTPRequestBuffer.append(BufferRef, RecvBytes);

    //- on incomplete (single) POST request
    if (_POSTWaitContentLength == true && _HTTPRequestBuffer.length() >= _POSTContentLength) {
        _RequestProperties.Payload = _HTTPRequestBuffer.substr(0, _POSTContentLength);
        _HTTPRequestBuffer.erase(0, _POSTContentLength);

        _Requests.push_back(_RequestProperties);

        _POSTWaitContentLength = false;
    }
    else {
        //- reset _SplittedRequests vector
        _SplittedRequests.clear();

        //- only process on minimum of 1 http request (end marker found)
        const size_t EndMarkerFound = _HTTPRequestBuffer.find(HTTP_1_1_END_MARKER);

        if (EndMarkerFound != string::npos) {
            _processRequests();
        }
    }
}

const RequestsMap_t& HTTPParser::getRequests() const
{
    return _Requests;
}

RequestsMapPtr_t HTTPParser::getRequestsPtr()
{
    return &_Requests;
}

inline void HTTPParser::_processRequests()
{
    //- split requests into _SplittedRequests vector
    StringHelper::split(_HTTPRequestBuffer, HTTP_1_1_END_MARKER, _SplittedRequests);

    //- iterate over splitted requests
    for(size_t i=0; i<_SplittedRequests.size(); ++i) {
        if (_processRequestProperties(i) == false) {
            _RequestParseError = HTTP_ERROR_BAD_REQUEST;
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
    _RequestProperties = {
        .HTTPVersion = HTTP_VERSION_UNKNOWN,
        .HTTPMethod = HTTP_METHOD_OTHER,
        .URL = "/"
    };

    //- parse base properties
    if (_parseRequestProperties(Request, _RequestProperties) == false) { return false; }

    //- only process HTTP/1.1 requests
    if (_RequestProperties.HTTPVersion != HTTP_VERSION_1_1) { return false; }

    //- if not GET || POST method, return
    if (_RequestProperties.HTTPMethod == HTTP_METHOD_OTHER) { return false; }

    //- parse request headers
    _parseRequestHeaders(Request, _RequestProperties.RequestHeaders);

    //- GET request
    if (_RequestProperties.HTTPMethod == HTTP_METHOD_GET) {
        //- parse GET parameters
        _parseGETParameter(_RequestProperties.URL, _RequestProperties.URLParams);

        //- add request to requests map
        _Requests.push_back(_RequestProperties);
    }

    //- POST request
    else if (_RequestProperties.HTTPMethod == HTTP_METHOD_POST) {

        //- if request does not contain content-length header
        if (_RequestProperties.RequestHeaders.find(HTTP_HEADER_CONTENT_LENGTH) == _RequestProperties.RequestHeaders.end()) {
            return false;
        }

        //- if content-length header contains non-digits
        if (StringHelper::is_digits(_RequestProperties.RequestHeaders.at(HTTP_HEADER_CONTENT_LENGTH)) == false) {
            return false;
        }

        _POSTContentLength = stoi(_RequestProperties.RequestHeaders.at(HTTP_HEADER_CONTENT_LENGTH));

        //- if content-length exeeds maximum
        if (_POSTContentLength > HTTP_POST_MAX_CONTENT_LENGTH) {
            return false;
        }

        //- on single HTTP POST: payload after endmarker in _HTTPRequestBuffer
        if (_SplittedRequests.size() == 1) {
            //- content length bytes already in buffer
            if (_HTTPRequestBuffer.length() >= _POSTContentLength) {
                _RequestProperties.Payload = _HTTPRequestBuffer.substr(0, _POSTContentLength);
                _HTTPRequestBuffer.erase(0, _POSTContentLength);

                //- add request to requests map
                _Requests.push_back(_RequestProperties);

            }
            else {
                //- set flag to wait until content-length reached
                _POSTWaitContentLength = true;
            }
        }
        //- otherwise payload in next splitted message
        else if (_SplittedRequests.size() > Index) {
            auto &NextRequest = _SplittedRequests.at(Index+1);
            if (NextRequest.length() >= _POSTContentLength) {
                _RequestProperties.Payload = NextRequest.substr(0, _POSTContentLength);
                NextRequest.erase(0, _POSTContentLength);
                //- add request to requests map
                _Requests.push_back(_RequestProperties);
            }
            else {
                return false;
            }
        }
    }
    return true;
}

inline bool HTTPParser::_parseRequestProperties(string_view Request, const RequestPropertiesRef_t ResultBaseProps)
{
    //- find first line endline
    size_t StartPos = Request.find("\r\n");

    //-> if no headers (no \r\n), set start pos to end of string
    if (StartPos == string_view::npos) {
        StartPos = Request.length();
    }

    //- get base request properties
    vector<string> SplitResult;
    StringHelper::rsplit(Request, StartPos, " ", SplitResult);

    //- on invalid vector size (!=3) abort
    if (SplitResult.size() != 3) { return false; }

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

    return true;
}

inline void HTTPParser::_parseRequestHeaders(string_view Request, RequestHeaderRef_t ResultRef)
{
    for (const auto line : StringHelper::linesOf(Request)) {
        const size_t colonSpace = line.find(": ");
        if (colonSpace != string_view::npos && colonSpace > 0) {
            const string_view value = line.substr(colonSpace + 2);
            if (!value.empty()) {
                ResultRef.emplace(string(line.substr(0, colonSpace)), string(value));
            }
        }
    }
}

inline void HTTPParser::_parseGETParameter(string_view RequestURL, URLParamMapRef_t ResultRef)
{
    //- only process on init character "?" found
    const size_t URLParamsStartPos = RequestURL.find("?");

    if (URLParamsStartPos != string_view::npos && RequestURL.length() > URLParamsStartPos) {

        const string_view URLParamsPart = RequestURL.substr(URLParamsStartPos + 1);

        for (const auto ParamValuePair : StringHelper::splitView(URLParamsPart, "&")) {

            const size_t PVPDelimiterPos = ParamValuePair.find("=");

            if (PVPDelimiterPos != string_view::npos && PVPDelimiterPos != 0 && ParamValuePair.length() > PVPDelimiterPos) {
                ResultRef.emplace(
                    string(ParamValuePair.substr(0, PVPDelimiterPos)),
                    string(ParamValuePair.substr(PVPDelimiterPos + 1))
                );
            }
        }
    }
}
