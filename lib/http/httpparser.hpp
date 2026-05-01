#pragma once

#include "httpconstants.hpp"

#include <string>
#include <vector>
#include <utility>
#include <algorithm>
#include <unordered_map>

#include <iostream>
#include <sstream>

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

typedef vector<RequestProperties_t> RequestsVector_t;


class HTTPParser
{

public:

    HTTPParser(const uint16_t);
    ~HTTPParser();

    void appendBuffer(const char*, const uint16_t);
    RequestsVector_t getRequests();
    void getNextRequest(const RequestPropertiesRef_t);

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
    RequestsVector_t _Requests;

protected:

    void _parseRequestProperties(string&, const RequestPropertiesRef_t);
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

    static bool is_digits(const string& checkdigits)
    {
        return all_of(checkdigits.begin(), checkdigits.end(), ::isdigit);
    }

};

class JSON {

public:

    static void getRequest2JSON()
    {
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
};
