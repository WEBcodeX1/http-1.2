#pragma once

#include <string>
#include <sstream>
#include <chrono>
#include <ctime>
#include <iomanip>
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

    void setStatus(const uint16_t, const string&);
    void setBodyRef(char*, const unsigned int);
    void addHeader(const HeaderID_t, const HeaderValue_t);
    void addDateHeader();
    void generate();

    char* _HeaderPointer;
    char* _BodyPointer;

private:

    uint16_t _StatusCode;
    string _StatusText;

    uint16_t _HeaderLength;
    unsigned int _BodyLength;

    char _HeaderBuffer[254];

    ResponseHeader_t Headers;

};
