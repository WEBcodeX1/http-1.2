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

struct SendMetadata_t {
    unsigned char* BufferRef;
    unsigned int BufferSize;
};

constexpr uint8_t SEND_STATE_IDLE = 0;
constexpr uint8_t SEND_STATE_SENDING = 1;
constexpr uint8_t SEND_TYPE_HEADER = 1;
constexpr uint8_t SEND_TYPE_BODY = 2;


class HTTPGenerator
{

public:

    HTTPGenerator();
    ~HTTPGenerator();

    void MsgReset();
    void MsgSetStatus(const uint16_t, const string&);
    void MsgSetSendStatus(const uint8_t);
    void MsgSetBodyRef(const unsigned char*, const unsigned int);
    void MsgAddHeader(const HeaderID_t, const HeaderValue_t);
    void MsgAddDateHeader();
    void MsgGenerate();

    //- test wrapper
    void generate();

    uint8_t MsgGetSendStatus();
    SendMetadata_t MsgGetSendMetadata();
    bool MsgUpdateSendMetadata(ssize_t);

private:

    uint8_t _SendStatus;
    uint8_t _SendType;

    uint16_t _StatusCode;
    string _StatusText;

    unsigned int _HeaderLength;
    unsigned int _BodyLength;

    unsigned int _HeaderRemainingBytes;
    unsigned int _BodyRemainingBytes;

    unsigned char* _HeaderPointer;
    unsigned char* _BodyPointer;

    unsigned char _HeaderBuffer[254];

    ResponseHeader_t Headers;

};
