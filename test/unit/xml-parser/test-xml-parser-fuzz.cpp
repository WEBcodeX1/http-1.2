#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

#include <memory>
#include <random>
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

string mutateOneCharacter(const string& Seed, mt19937& RNG)
{
    if (Seed.empty()) {
        return Seed;
    }

    uniform_int_distribution<size_t> IndexDist(0, Seed.size() - 1);
    uniform_int_distribution<int> ByteDist(33, 126);

    string Output = Seed;
    Output[IndexDist(RNG)] = static_cast<char>(ByteDist(RNG));
    return Output;
}

const string SEED_XML =
    "<NLAP>"
    "<Request>"
    "<UUID>123e4567-e89b-12d3-a456-426614174000</UUID>"
    "<Protocol>NLAP</Protocol>"
    "<Version>0.1</Version>"
    "<Subtype>NLAMP</Subtype>"
    "<Header><Host>testapp1.local</Host></Header>"
    "</Request>"
    "</NLAP>";

} // namespace

BOOST_AUTO_TEST_CASE(xml_parser_fuzz_like_mutation_sweep)
{
    unique_ptr<XMLParser> Parser = make_unique<XMLParser>(8192);
    mt19937 RNG(42);

    for (size_t Iteration = 0; Iteration < 200; ++Iteration) {
        const string Mutated = mutateOneCharacter(SEED_XML, RNG);
        vector<char> Buffer = toMutableBuffer(Mutated);
        ParseResult_t Result = Parser->parse(Buffer.data());

        const bool IsKnownResult =
            Result.ErrorCode == 0 ||
            Result.ErrorCode == XML_ERROR_INVALID_CONTENT_DTD ||
            Result.ErrorCode == XML_ERROR_INVALID_SYNTAX ||
            Result.ErrorCode == XML_ERROR_INVALID_FRAMING;

        BOOST_TEST(IsKnownResult);
    }
}
