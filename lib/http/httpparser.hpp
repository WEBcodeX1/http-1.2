#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include "httpconstants.hpp"

typedef pair<string, string> HeaderPair_t;
typedef unordered_map<string, string> RequestHeader_t;

typedef RequestHeader_t RequestHeaderResult_t;
typedef RequestHeader_t& RequestHeaderResultRef_t;

typedef unordered_map<string, string> URLParam_t;

struct BaseProperties_t
{
    uint16_t HTTPVersion;
    uint16_t HTTPMethod;
    string URL;
}

typedef BaseProperties_t& BasePropertiesRef_t;

struct RequestStruct_t
{
    BaseProps_t BaseProperties;
    RequestHeader_t RequestHeaders;
    string Payload;
    URLParam_t URLParams;
}

static const vector<string> HTTPHeaderTypes
{
    "Host",
    "Transfer-Encoding",
    "If-None-Match",
    "Content-Type",
    "Content-Length"
};


class HTTPParser
{

public:

    HTTPParser();
    ~HTTPParser();

    void appendBuffer(const char*, const uint16_t);
    size_t processRequests();

private:

    inline void _splitRequests();
    void _processRequestProperties(const size_t);

    RequestHeader_t _RequestHeaders;
    vector<string> _SplittedRequests;

    size_t _RequestCount;
    size_t _RequestCountGet;
    size_t _RequestCountPost;
    size_t _RequestCountPostAS;

    uint16_t _RequestNumber;

    string _HTTPRequestBuffer;

protected:

    void _parseRequestProperties(string&, BasePropsResultRef_t);
    void _parseRequestHeaders(string&, RequestHeaderResultRef_t);

    inline uint16_t _getURLParamValue(
        const string&,
        const uint16_t,
        string&
    );
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
        StartPos -= Delimiter.length();
        while ((FindPos = String.rfind(Delimiter, StartPos)) != String.npos) {
            Token = String.substr(FindPos+Delimiter.length(), (StartPos-FindPos));
            ResultRef.push_back(Token);
            StartPos = FindPos - Delimiter.length();
            FindPosLast = FindPos;
        }
        Token = String.substr(0, FindPosLast);
        ResultRef.push_back(Token);
    }

};
