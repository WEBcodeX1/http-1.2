#include "httpgenerator.hpp"
#include "httpconstants.hpp"

using namespace std;


HTTPGenerator::HTTPGenerator() :
    _StatusCode(200),
    _StatusText("OK")
{
}

HTTPGenerator::~HTTPGenerator()
{
}

void HTTPGenerator::setStatus(const uint16_t StatusCode, const string& StatusText)
{
    _StatusCode = StatusCode;
    _StatusText = StatusText;
}

void HTTPGenerator::addHeader(const HeaderID_t HeaderID, const HeaderValue_t HeaderValue)
{
    Headers.emplace(HeaderID, HeaderValue);
}

void HTTPGenerator::addDateHeader()
{
    stringstream current_date;
    std::time_t tt = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    struct std::tm * ptm = std::gmtime(&tt);
    current_date << std::put_time(ptm, "%a, %d %b %Y %T GMT");
    addHeader("Date", current_date.str());
}

void HTTPGenerator::setBodyRef(char* BodyAddress, const unsigned int BodyLength)
{
    _BodyPointer = BodyAddress;
    _BodyLength = BodyLength;
}

void HTTPGenerator::generate()
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

    Message.copy(_HeaderBuffer, _HeaderLength);
}
