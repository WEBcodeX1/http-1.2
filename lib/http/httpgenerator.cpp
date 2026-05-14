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

void HTTPGenerator::addHeader(HeaderID_t HeaderID, HeaderValue_t HeaderValue)
{
    Headers.emplace(HeaderID, HeaderValue);
}

void HTTPGenerator::addDateHeader()
{
    stringstream current_date;
    std::time_t tt = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    struct std::tm * ptm = std::localtime(&tt);
    current_date << std::put_time(ptm, "%a, %d %b %Y %T") << '\n';
    string DateValue = current_date.str();
    if (!DateValue.empty() && DateValue.back() == '\n')
        DateValue.pop_back();
    addHeader("Date", DateValue);
}

string HTTPGenerator::generate()
{
    string Message = "HTTP/1.1 " + to_string(_StatusCode) + " " + _StatusText + "\r\n";

    for (const auto& Header : Headers) {
        const string& Name = Header.first;
        const string& Value = Header.second;

        if (Name.find('\r') != string::npos || Name.find('\n') != string::npos ||
            Value.find('\r') != string::npos || Value.find('\n') != string::npos) {
            continue;
        }

        Message += Name + ": " + Value + "\r\n";
    }

    Message += "Content-Length: " + to_string(_Body.length()) + "\r\n";
    Message += "\r\n";
    Message += _Body;

    return Message;
}
