#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

#include <memory>
#include <string>
#include <vector>

#include "../../../lib/xml/xmlconstants.hpp"
#include "../../../lib/xml/xmlparser.hpp"

using namespace std;

namespace
{

vector<char> toMutableBuffer(const string& Input)
{
    vector<char> Buffer(Input.begin(), Input.end());
    Buffer.push_back('\0');
    return Buffer;
}

const string VALID_REQUEST =
    "<NLAP>"
    "<Request>"
    "<UUID>123e4567-e89b-12d3-a456-426614174000</UUID>"
    "<Protocol>NLAP</Protocol>"
    "<Version>0.1</Version>"
    "<Subtype>NLAMP</Subtype>"
    "<Header>"
    "<Host>testapp1.local</Host>"
    "<URL>/testpath/index.html</URL>"
    "<User-Agent>Falcon-Browser</User-Agent>"
    "</Header>"
    "</Request>"
    "</NLAP>";

const string VALID_RESPONSE =
    "<NLAP>"
    "<Response>"
    "<UUID>123e4567-e89b-12d3-a456-426614174001</UUID>"
    "<Protocol>NLAP</Protocol>"
    "<Version>0.1</Version>"
    "<Subtype>NLAMP</Subtype>"
    "<Header><Mime-Type>text/plain</Mime-Type></Header>"
    "<Status><Code>0</Code><Description>ok</Description></Status>"
    "</Response>"
    "</NLAP>";

} // namespace

BOOST_AUTO_TEST_CASE(parse_single_message_and_validate_leaf_pointer)
{
    unique_ptr<XMLParser> Parser = make_unique<XMLParser>(4096);
    vector<char> Buffer = toMutableBuffer(VALID_REQUEST);

    ParseResult_t Result = Parser->parse(Buffer.data());

    BOOST_TEST(Result.ErrorCode == 0);
    BOOST_TEST(Result.Results.size() == 1u);

    const XMLNode& URLNode = Result.Results.at(0).at("NLAP").at("Request").at("Header").at("URL");
    BOOST_TEST(URLNode.Address != nullptr);
    BOOST_TEST(URLNode.Length == string("/testpath/index.html").size());

    string URL(URLNode.Address, URLNode.Length);
    BOOST_TEST(URL == "/testpath/index.html");
    BOOST_TEST(URLNode.Address >= Buffer.data());
    BOOST_TEST(URLNode.Address < (Buffer.data() + Buffer.size()));
}

BOOST_AUTO_TEST_CASE(parse_multiple_messages_returns_vector)
{
    unique_ptr<XMLParser> Parser = make_unique<XMLParser>(8192);
    vector<char> Buffer = toMutableBuffer(VALID_REQUEST + VALID_RESPONSE);

    ParseResult_t Result = Parser->parse(Buffer.data());

    BOOST_TEST(Result.ErrorCode == 0);
    BOOST_TEST(Result.Results.size() == 2u);
}

BOOST_AUTO_TEST_CASE(parse_invalid_syntax_returns_error)
{
    unique_ptr<XMLParser> Parser = make_unique<XMLParser>(4096);

    const string Broken = "<NLAP><Request><UUID>x</UUID><Protocol>NLAP</Protocol></Request>";
    vector<char> Buffer = toMutableBuffer(Broken);

    ParseResult_t Result = Parser->parse(Buffer.data());

    BOOST_TEST(Result.ErrorCode == XML_ERROR_INVALID_SYNTAX);
    BOOST_TEST(Result.Results.empty());
}

BOOST_AUTO_TEST_CASE(parse_invalid_dtd_content_returns_error)
{
    unique_ptr<XMLParser> Parser = make_unique<XMLParser>(4096);

    const string InvalidDTD =
        "<NLAP><Request><Protocol>NLAP</Protocol><Version>0.1</Version><Subtype>NLAMP</Subtype><Header></Header></Request></NLAP>";
    vector<char> Buffer = toMutableBuffer(InvalidDTD);

    ParseResult_t Result = Parser->parse(Buffer.data());

    BOOST_TEST(Result.ErrorCode == XML_ERROR_INVALID_CONTENT_DTD);
    BOOST_TEST(Result.Results.empty());
}

BOOST_AUTO_TEST_CASE(parse_invalid_framing_returns_error)
{
    unique_ptr<XMLParser> Parser = make_unique<XMLParser>(8192);

    const string Framed = VALID_REQUEST + string("BAD") + VALID_RESPONSE;
    vector<char> Buffer = toMutableBuffer(Framed);

    ParseResult_t Result = Parser->parse(Buffer.data());

    BOOST_TEST(Result.ErrorCode == XML_ERROR_INVALID_FRAMING);
    BOOST_TEST(Result.Results.empty());
}

BOOST_AUTO_TEST_CASE(parse_buffer_size_runtime_override)
{
    unique_ptr<XMLParser> Parser = make_unique<XMLParser>(32);

    BOOST_TEST(Parser->getParseBufferSize() == 32u);

    Parser->setParseBufferSize(8192);
    BOOST_TEST(Parser->getParseBufferSize() == 8192u);

    vector<char> Buffer = toMutableBuffer(VALID_REQUEST);
    ParseResult_t Result = Parser->parse(Buffer.data());

    BOOST_TEST(Result.ErrorCode == 0);
    BOOST_TEST(Result.Results.size() == 1u);
}
