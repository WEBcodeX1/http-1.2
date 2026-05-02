#pragma once

#include "httpconstants.hpp"

#include <string>
#include <vector>
#include <memory>
#include <utility>
#include <algorithm>
#include <unordered_map>

using namespace std;

typedef unordered_map<string, string> RequestHeader_t;
typedef RequestHeader_t& RequestHeaderRef_t;

typedef unordered_map<string, string> URLParamMap_t;
typedef URLParamMap_t& URLParamMapRef_t;

struct RequestProperties_t
{
    uint16_t HTTPVersion;
    uint16_t HTTPMethod;
    RequestHeader_t RequestHeaders;
    string URL;
    string Payload;
    URLParamMap_t URLParams;
};

typedef RequestProperties_t& RequestPropertiesRef_t;
typedef shared_ptr<RequestProperties_t> RequestPropertiesPtr_t;

typedef unordered_map<uint16_t, RequestProperties_t> RequestsMap_t;


class HTTPParser
{

public:

    HTTPParser(const uint16_t);
    ~HTTPParser();

    void appendBuffer(const char*, const uint16_t);
    RequestsMap_t getRequests();
    RequestPropertiesPtr_t getNextRequest();
    void removeRequest(uint16_t);

private:

    void _processRequests();
    bool _processRequestProperties(const size_t);

    RequestHeader_t _RequestHeaders;
    vector<string> _SplittedRequests;

    size_t _RequestCountGet;
    size_t _RequestCountPost;

    uint16_t _RequestParseError;

    bool _POSTWaitContentLength;
    uint16_t _POSTContentLength;

    uint16_t _ReqAddIndex;
    uint16_t _ReqNextIndex;

    string _HTTPRequestBuffer;

    uint16_t _HTTPRequestBufferMax;

    RequestProperties_t _RequestProperties;
    RequestsMap_t _Requests;

protected:

    bool _parseRequestProperties(string&, const RequestPropertiesRef_t);
    void _parseRequestHeaders(string&, RequestHeaderRef_t);
    void _parseGETParameter(const string&, URLParamMapRef_t);
};


class StringHelper {

public:

    static void split(string& StringRef, const string Delimiter, vector<string>& ResultRef)
    {
        string SplitElement;
        auto pos = StringRef.find(Delimiter);

        while (pos != string::npos) {
            SplitElement = StringRef.substr(0, pos);
            ResultRef.push_back(SplitElement);
            StringRef.erase(0, pos + Delimiter.length());
            pos = StringRef.find(Delimiter);
        }
    }

    static void rsplit(string& String, size_t StartPos, const string Delimiter, vector<string>& ResultRef)
    {
        size_t FindPos = 0;
        size_t FindPosLast = 0;
        string Token;
        if (StartPos < Delimiter.length()) {
            ResultRef.push_back(String.substr(0, StartPos));
            return;
        }
        StartPos -= Delimiter.length();
        while ((FindPos = String.rfind(Delimiter, StartPos)) != String.npos) {
            Token = String.substr(FindPos+Delimiter.length(), (StartPos-FindPos));
            ResultRef.push_back(Token);
            if (FindPos < Delimiter.length()) {
                FindPosLast = FindPos;
                break;
            }
            StartPos = FindPos - Delimiter.length();
            FindPosLast = FindPos;
        }
        Token = String.substr(0, FindPosLast);
        ResultRef.push_back(Token);
    }

    static bool is_digits(const string& checkdigits)
    {
        return all_of(checkdigits.begin(), checkdigits.end(), ::isdigit);
    }

};

class JSON {

public:

    static void URLParamsMap2JSON(URLParamMapRef_t URLParamsMap, string& JSONPayload)
    {
        JSONPayload = "{ \"payload\": {";

        uint16_t i = 0;
        for (const auto& [ParamName, ParamValue]: URLParamsMap) {
            JSONPayload += " \"" + ParamName + "\": \"" + ParamValue + "\"";
            if (i != URLParamsMap.size()-1) {
                JSONPayload += ",";
            }
            ++i;
        }
        JSONPayload += " }}";
    }
};
