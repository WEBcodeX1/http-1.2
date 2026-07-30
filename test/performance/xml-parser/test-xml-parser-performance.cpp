#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

#include <chrono>
#include <memory>
#include <string>
#include <vector>

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

} // namespace

BOOST_AUTO_TEST_CASE(xml_parser_minor_performance_smoke)
{
    const string Payload =
        "<NLAP><Request><UUID>perf-uuid</UUID><Protocol>NLAP</Protocol><Version>0.1</Version><Subtype>NLAMP</Subtype>"
        "<Header><Host>perf.local</Host><URL>/perf</URL><User-Agent>perf-client</User-Agent></Header>"
        "</Request></NLAP>";

    vector<char> Buffer = toMutableBuffer(Payload);
    unique_ptr<XMLParser> Parser = make_unique<XMLParser>(4096);

    const auto Start = chrono::steady_clock::now();

    for (size_t Iteration = 0; Iteration < 1000; ++Iteration) {
        ParseResult_t Result = Parser->parse(Buffer.data());
        BOOST_TEST(Result.ErrorCode == 0);
        BOOST_TEST(Result.Results.size() == 1u);
    }

    const auto End = chrono::steady_clock::now();
    const auto DurationUS = chrono::duration_cast<chrono::microseconds>(End - Start).count();

    BOOST_TEST(DurationUS > 0);
}
