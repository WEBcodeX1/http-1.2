#pragma once

#include <string>
#include <unordered_map>
#include <cstdint>

using namespace std;

typedef unordered_map<string, string> ResponseHeader_t;
typedef ResponseHeader_t& ResponseHeaderRef_t;

class HTTPGenerator
{

public:

    HTTPGenerator();
    ~HTTPGenerator();

    void setStatus(const uint16_t StatusCode, const string& StatusText);
    void setBody(const string& Body);
    string generate();

    ResponseHeader_t headers;

private:

    uint16_t _StatusCode;
    string _StatusText;
    string _Body;

};
