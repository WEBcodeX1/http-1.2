#include "httpgenerator.hpp"
#include "httpconstants.hpp"

using namespace std;


HTTPGenerator::HTTPGenerator() :
    _SendStatus(SEND_STATE_IDLE),
    _StatusCode(200),
    _StatusText("OK")
{
}

HTTPGenerator::~HTTPGenerator()
{
}

void HTTPGenerator::MsgReset()
{
    _StatusCode = 200;
    _StatusText = "OK";
    _HeaderPointer = &_HeaderBuffer[0];
    _BodyPointer = nullptr;
    _HeaderLength = 0;
    _BodyLength = 0;
    _SendStatus = SEND_STATE_IDLE;
    _SendType = SEND_TYPE_HEADER;
    Headers.clear();
}

void HTTPGenerator::MsgSetStatus(const uint16_t StatusCode, const string& StatusText)
{
    _StatusCode = StatusCode;
    _StatusText = StatusText;
}

void HTTPGenerator::MsgSetSendStatus(const uint8_t SendStatus)
{
    _SendStatus = SendStatus;
}

uint8_t HTTPGenerator::MsgGetSendStatus()
{
    return _SendStatus;
}

void HTTPGenerator::MsgAddHeader(const HeaderID_t HeaderID, const HeaderValue_t HeaderValue)
{
    Headers.emplace(HeaderID, HeaderValue);
}

void HTTPGenerator::MsgAddDateHeader()
{
    stringstream current_date;
    std::time_t tt = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    struct std::tm * ptm = std::gmtime(&tt);
    current_date << std::put_time(ptm, "%a, %d %b %Y %T GMT");
    MsgAddHeader("Date", current_date.str());
}

void HTTPGenerator::MsgSetBodyRef(const unsigned char* BodyAddress, const unsigned int BodyLength)
{
    _BodyPointer = const_cast<unsigned char*>(BodyAddress);
    _BodyLength = BodyLength;
    _BodyRemainingBytes = _BodyLength;
}

void HTTPGenerator::MsgGenerate()
{
    string Message = "HTTP/1.1 " + to_string(_StatusCode) + " " + _StatusText + "\r\n";

    for (const auto& Header : Headers)
    {
        const string& Name = Header.first;
        const string& Value = Header.second;

        Message += Name + ": " + Value + "\r\n";
    }

    Message += "Content-Length: " + to_string(_BodyLength) + "\r\n";
    Message += "\r\n";

    _HeaderLength = Message.length();
    _HeaderRemainingBytes = _HeaderLength;

    Message.copy((char*)_HeaderBuffer, _HeaderLength);
}

SendMetadata_t HTTPGenerator::MsgGetSendMetadata()
{
    if (_SendType == SEND_TYPE_HEADER) {
        return { _HeaderPointer, _HeaderRemainingBytes };
    }
    else if (_SendType == SEND_TYPE_BODY) {
        return { _BodyPointer, _BodyRemainingBytes };
    }
    return { nullptr, 0 };
}

bool HTTPGenerator::MsgUpdateSendMetadata(ssize_t SentBytes)
{
    if (_SendType == SEND_TYPE_HEADER && _HeaderRemainingBytes >= SentBytes) {
        _HeaderPointer += SentBytes;
        _HeaderRemainingBytes -= SentBytes;
        if (_HeaderRemainingBytes == SentBytes) {
            _SendType = SEND_TYPE_BODY;
            _HeaderRemainingBytes = 0;
        }
    }
    else if (_SendType == SEND_TYPE_BODY && _BodyRemainingBytes >= SentBytes) {
        _BodyPointer += SentBytes;
        _BodyRemainingBytes -= SentBytes;
        if (_BodyRemainingBytes == SentBytes) {
            _BodyRemainingBytes = 0;
            return true;
        }
    }
    return false;
}
