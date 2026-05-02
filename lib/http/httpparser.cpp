#include "httpparser.hpp"

using namespace std;


HTTPParser::HTTPParser(const uint16_t BufferSize) :
    _RequestCountGet(0),
    _RequestCountPost(0),
    _RequestParseError(0),
    _POSTWaitContentLength(false),
    _POSTContentLength(0),
    _ReqAddIndex(0),
    _ReqNextIndex(0),
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
        _HTTPRequestBuffer.replace(0, _POSTContentLength, "");
        //_Requests.push_back(_RequestProperties);
        _Requests.emplace(_ReqAddIndex, _RequestProperties);
        _ReqAddIndex += 1;
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

RequestsMap_t HTTPParser::getRequests()
{
    return _Requests;
}

RequestPropertiesPtr_t HTTPParser::getNextRequest()
{
    if (_Requests.size() > 0) {
        _ReqNextIndex += 1;
        return make_shared<RequestProperties_t>(_Requests.at(_ReqNextIndex-1));
    }
    return nullptr;
}

void HTTPParser::removeRequest(uint16_t Index)
{
    if (_Requests.find(Index) != _Requests.end()) {
        _Requests.erase(Index);
    }
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
        //_Requests.push_back(_RequestProperties);
        _Requests.emplace(_ReqAddIndex, _RequestProperties);
        _ReqAddIndex += 1;
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
                _HTTPRequestBuffer.replace(0, _POSTContentLength, "");
                //- add request to requests map
                //_Requests.push_back(_RequestProperties);
                _Requests.emplace(_ReqAddIndex, _RequestProperties);
                _ReqAddIndex += 1;
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
                NextRequest.replace(0, _POSTContentLength, "");
                //- add request to requests map
                //_Requests.push_back(_RequestProperties);
                _Requests.emplace(_ReqAddIndex, _RequestProperties);
                _ReqAddIndex += 1;
            }
            else {
                return false;
            }
        }
    }
    return true;
}

inline bool HTTPParser::_parseRequestProperties(string& Request, const RequestPropertiesRef_t ResultBaseProps)
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

inline void HTTPParser::_parseRequestHeaders(string& Request, RequestHeaderRef_t ResultRef)
{
    //- split / reverse split header lines
    vector<string> Lines;
    StringHelper::split(Request, "\r\n", Lines);

    Lines.push_back(Request);

    //- loop over lines, split, put into result map
    for (auto &Line:Lines) {

        vector<string> HeaderPair;
        if (Line.find(":") != string::npos) {

            StringHelper::rsplit(Line, Line.length(), ": ", HeaderPair);

            if (HeaderPair.size() == 2 && !HeaderPair.at(1).empty()) {
                ResultRef.emplace(
                    HeaderPair.at(1), HeaderPair.at(0).substr(0, HeaderPair.at(0).length())
                );
            }
        }
    }
}

inline void HTTPParser::_parseGETParameter(const string& RequestURL, URLParamMapRef_t ResultRef)
{
    //- only process on init character "?" found
    const size_t URLParamsStartPos = RequestURL.find("?");

    if (URLParamsStartPos != string::npos && RequestURL.length() > URLParamsStartPos) {

        string URLParamsPart = RequestURL.substr(URLParamsStartPos+1, RequestURL.length());

        vector<string> ParamValuePairs;
        StringHelper::split(URLParamsPart, "&", ParamValuePairs);

        ParamValuePairs.push_back(URLParamsPart);

        //- loop over param-value pairs
        for (auto &ParamValuePair:ParamValuePairs) {

            const size_t PVPDelimiterPos = ParamValuePair.find("=");

            if (PVPDelimiterPos != string::npos && PVPDelimiterPos != 0 && ParamValuePair.length() > PVPDelimiterPos) {
                ResultRef.emplace(
                    ParamValuePair.substr(0, PVPDelimiterPos), ParamValuePair.substr(PVPDelimiterPos+1, ParamValuePair.length())
                );
            }
        }
    }
}
