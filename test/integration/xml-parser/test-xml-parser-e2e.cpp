#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

#include <cstring>
#include <memory>
#include <string>

#include "../../../lib/xml/xmlparser.hpp"

using namespace std;

BOOST_AUTO_TEST_CASE(end_to_end_parse_request_and_response_stream)
{
    char InputBuffer[4096] = {0};

    const string Request =
        "<NLAP><Request><UUID>request-1</UUID><Protocol>NLAP</Protocol><Version>0.1</Version><Subtype>NLAMP</Subtype>"
        "<Header><Host>testapp1.local</Host><URL>/health</URL><User-Agent>Falcon</User-Agent></Header>"
        "</Request></NLAP>";

    const string Response =
        "<NLAP><Response><UUID>response-1</UUID><Protocol>NLAP</Protocol><Version>0.1</Version><Subtype>NLAMP</Subtype>"
        "<Header><Mime-Type>application/json</Mime-Type></Header><Payload>{\"ok\":true}</Payload>"
        "<Status><Code>0</Code><Description>ok</Description></Status>"
        "</Response></NLAP>";

    const string Stream = Request + Response;
    memcpy(InputBuffer, Stream.c_str(), Stream.size());

    unique_ptr<XMLParser> Parser = make_unique<XMLParser>(4096);
    ParseResult_t Result = Parser->parse(InputBuffer);

    BOOST_TEST(Result.ErrorCode == 0);
    BOOST_TEST(Result.Results.size() == 2u);

    const XMLNode& ReqHost = Result.Results.at(0).at("NLAP").at("Request").at("Header").at("Host");
    const XMLNode& ResCode = Result.Results.at(1).at("NLAP").at("Response").at("Status").at("Code");

    BOOST_TEST(string(ReqHost.Address, ReqHost.Length) == "testapp1.local");
    BOOST_TEST(string(ResCode.Address, ResCode.Length) == "0");
}
