#pragma once

#include <string>
#include <unordered_map>
#include <cstdint>

using namespace std;

typedef string HeaderID_t;
typedef string HeaderValue_t;
typedef unordered_map<HeaderID_t, HeaderValue_t> ResponseHeader_t;

class HTTPGenerator
{

public:

    HTTPGenerator();
    ~HTTPGenerator();

    void setStatus(const uint16_t StatusCode, const string& StatusText);
    void setBody(const string& Body);
    void addHeader(HeaderID_t HeaderID, HeaderValue_t HeaderValue);
    string generate();

private:

    uint16_t _StatusCode;
    string _StatusText;
    string _Body;

    ResponseHeader_t Headers;

};
