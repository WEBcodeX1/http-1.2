#pragma once

//- Legacy C++11 compatible HTTP parser header.
//- Drop-in replacement for httpparser.hpp when targeting -std=c++11 or later.
//- Does not use string_view (C++17), span (C++20), or ispanstream (C++23).
//- Transparent unordered_map lookup (is_transparent / equal_to<>) is not used;
//- map key lookups construct a temporary std::string as in pre-C++14 code.

#include <string>
#include <vector>
#include <cstdint>
#include <utility>
#include <algorithm>
#include <unordered_map>

using namespace std;

using RequestHeader_t    = unordered_map<string, string>;
using RequestHeaderRef_t = RequestHeader_t&;

using URLParamMap_t    = unordered_map<string, string>;
using URLParamMapRef_t = URLParamMap_t&;

struct RequestProperties_t
{
    uint16_t HTTPVersion;
    uint16_t HTTPMethod;
    RequestHeader_t RequestHeaders;
    string URL;
    string Payload;
    URLParamMap_t URLParams;
};

using RequestPropertiesRef_t = RequestProperties_t&;

using RequestsMap_t    = vector<RequestProperties_t>;
using RequestsMapPtr_t = RequestsMap_t*;


class HTTPParser
{

public:

    HTTPParser(const uint16_t);
    ~HTTPParser();

    void appendBuffer(const char*, const uint16_t);
    const RequestsMap_t& getRequests() const;
    RequestsMapPtr_t getRequestsPtr();

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

    string _HTTPRequestBuffer;

    uint16_t _HTTPRequestBufferMax;

    RequestProperties_t _RequestProperties;
    RequestsMap_t _Requests;

protected:

    bool _parseRequestProperties(const string&, const RequestPropertiesRef_t);
    void _parseRequestHeaders(const string&, RequestHeaderRef_t);
    void _parseGETParameter(const string&, URLParamMapRef_t);
};


class StringHelper {

public:

    //- Destructive split: erases consumed tokens from StringRef in-place (used for buffer management)
    static void split(string& StringRef, const string& Delimiter, vector<string>& ResultRef)
    {
        size_t pos = StringRef.find(Delimiter);
        while (pos != string::npos) {
            ResultRef.push_back(StringRef.substr(0, pos));
            StringRef.erase(0, pos + Delimiter.size());
            pos = StringRef.find(Delimiter);
        }
    }

    static void rsplit(const string& String, size_t StartPos, const string& Delimiter, vector<string>& ResultRef)
    {
        size_t FindPos = 0;
        size_t FindPosLast = 0;

        //- guard ensures StartPos >= Delimiter.length() before the subtraction below
        if (StartPos < Delimiter.length()) {
            ResultRef.push_back(String.substr(0, StartPos));
            return;
        }

        StartPos -= Delimiter.length();   //- safe: guarded by the check above

        while ((FindPos = String.rfind(Delimiter, StartPos)) != string::npos) {

            ResultRef.push_back(String.substr(FindPos + Delimiter.length(), StartPos - FindPos));

            //- guard ensures FindPos >= Delimiter.length() before the subtraction below
            if (FindPos < Delimiter.length()) {
                FindPosLast = FindPos;
                break;
            }

            StartPos = FindPos - Delimiter.length();   //- safe: guarded by the check above
            FindPosLast = FindPos;
        }

        ResultRef.push_back(String.substr(0, FindPosLast));
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
        for (const auto& ParamPair: URLParamsMap) {
            JSONPayload += " \"" + ParamPair.first + "\": \"" + ParamPair.second + "\"";
            if (i != URLParamsMap.size()-1) {
                JSONPayload += ",";
            }
            ++i;
        }
        JSONPayload += " }}";
    }
};
