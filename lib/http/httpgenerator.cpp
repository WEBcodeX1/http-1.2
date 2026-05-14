#include "httpgenerator.hpp"
#include "httpconstants.hpp"

using namespace std;


HTTPGenerator::HTTPGenerator() :
    _StatusCode(200),
    _StatusText("OK"),
    _Body("")
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

void HTTPGenerator::setBody(const string& Body)
{
    _Body = Body;
}

string HTTPGenerator::generate()
{
    string Message = "HTTP/1.1 " + to_string(_StatusCode) + " " + _StatusText + "\r\n";

    for (const auto& Header : headers) {
        Message += Header.first + ": " + Header.second + "\r\n";
    }

    if (!_Body.empty()) {
        Message += "Content-Length: " + to_string(_Body.length()) + "\r\n";
    }

    Message += "\r\n";
    Message += _Body;

    return Message;
}
