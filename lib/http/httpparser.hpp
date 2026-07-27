#pragma once

#include <string>
#include <string_view>
#include <span>
#include <vector>
#include <cstdint>
#include <utility>
#include <algorithm>
#include <unordered_map>

using namespace std;

//- transparent hasher: enables heterogeneous lookup (string_view keys without string construction)
struct StringHash {
    using is_transparent = void;
    size_t operator()(string_view sv) const noexcept {
        return hash<string_view>{}(sv);
    }
};

using RequestHeader_t    = unordered_map<string, string, StringHash, equal_to<>>;
using RequestHeaderRef_t = RequestHeader_t&;

using URLParamMap_t    = unordered_map<string, string, StringHash, equal_to<>>;
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

    bool _parseRequestProperties(string_view, const RequestPropertiesRef_t);
    void _parseRequestHeaders(string_view, RequestHeaderRef_t);
    void _parseGETParameter(string_view, URLParamMapRef_t);
};


class StringHelper {

public:

    //- Non-destructive split: returns string_view slices into sv (zero heap allocation per token)
    static void split(string_view sv, string_view delim, vector<string_view>& out)
    {
        for (size_t pos; (pos = sv.find(delim)) != sv.npos; sv.remove_prefix(pos + delim.size()))
            out.push_back(sv.substr(0, pos));
        if (!sv.empty()) out.push_back(sv);
    }

    //- Destructive split: erases consumed tokens from StringRef in-place (used for buffer management)
    static void split(string& StringRef, string_view Delimiter, vector<string>& ResultRef)
    {
        auto pos = StringRef.find(Delimiter);
        while (pos != string::npos) {
            ResultRef.push_back(StringRef.substr(0, pos));
            StringRef.erase(0, pos + Delimiter.size());
            pos = StringRef.find(Delimiter);
        }
    }

    static void rsplit(string_view String, size_t StartPos, string_view Delimiter, vector<string>& ResultRef)
    {
        size_t FindPos = 0;
        size_t FindPosLast = 0;

        if (StartPos < Delimiter.length()) {
            ResultRef.push_back(string(String.substr(0, StartPos)));
            return;
        }

        StartPos -= Delimiter.length();

        while ((FindPos = String.rfind(Delimiter, StartPos)) != string_view::npos) {

            ResultRef.push_back(string(String.substr(FindPos + Delimiter.length(), StartPos - FindPos)));

            if (FindPos < Delimiter.length()) {
                FindPosLast = FindPos;
                break;
            }

            StartPos = FindPos - Delimiter.length();
            FindPosLast = FindPos;
        }

        ResultRef.push_back(string(String.substr(0, FindPosLast)));
    }

    static bool is_digits(string_view checkdigits)
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
