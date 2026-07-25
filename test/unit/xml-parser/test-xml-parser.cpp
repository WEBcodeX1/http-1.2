#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>
#include <memory>
#include <iostream>
#include <string>
#include <string_view>
#include <cstring>

#include "../../../lib/xml/xmlparser.hpp"
#include "../../../lib/xml/xmlconstants.hpp"

using namespace std;

// ---------------------------------------------------------------------------
//  Valid NLAMP Request
// ---------------------------------------------------------------------------

static const string NLAMP_REQUEST_FULL(
    "<NLAP>"
    "<Request>"
    "<UUID>9b327afe-27ae-2367-aef2-e42445e5b23a</UUID>"
    "<Protocol>NLAP</Protocol>"
    "<Version>0.1</Version>"
    "<Subtype>NLAMP</Subtype>"
    "<Header>"
    "<Host>testapp2.local</Host>"
    "<URL>/python/service1</URL>"
    "<User-Agent>Falcon-Python-Client</User-Agent>"
    "</Header>"
    "<Payload>{\"param1\":\"string1\",\"param2\":\"string2\",\"param3\":100}</Payload>"
    "</Request>"
    "</NLAP>"
);

// ---------------------------------------------------------------------------
//  Valid NLAMP Response (success, JSON payload)
// ---------------------------------------------------------------------------

static const string NLAMP_RESPONSE_SUCCESS(
    "<NLAP>"
    "<Response>"
    "<UUID>9b327afe-27ae-2367-aef2-e42445e5b23a</UUID>"
    "<Protocol>NLAP</Protocol>"
    "<Version>0.1</Version>"
    "<Subtype>NLAMP</Subtype>"
    "<Header>"
    "<Mime-Type>application/json</Mime-Type>"
    "<Encoding>UTF-8</Encoding>"
    "</Header>"
    "<Payload>{ \"Result\": 100 }</Payload>"
    "<Status><Code>0</Code></Status>"
    "</Response>"
    "</NLAP>"
);

// ---------------------------------------------------------------------------
//  Valid NLAMP Response (failure, status with description + exception)
// ---------------------------------------------------------------------------

static const string NLAMP_RESPONSE_FAILURE(
    "<NLAP>"
    "<Response>"
    "<UUID>9b327afe-27ae-2367-aef2-e42445e5b23a</UUID>"
    "<Protocol>NLAP</Protocol>"
    "<Version>0.1</Version>"
    "<Subtype>NLAMP</Subtype>"
    "<Header>"
    "<Mime-Type>application/xml</Mime-Type>"
    "<Encoding>UTF-8</Encoding>"
    "</Header>"
    "<Status>"
    "<Code>10</Code>"
    "<Description>Application Exception</Description>"
    "<Exception>NameError: name test is not defined</Exception>"
    "</Status>"
    "</Response>"
    "</NLAP>"
);

// ---------------------------------------------------------------------------
//  Valid NLAFP Request (static HTML file)
// ---------------------------------------------------------------------------

static const string NLAFP_REQUEST_STATIC_FILE(
    "<NLAP>"
    "<Request>"
    "<UUID>7ea45c8a-5193-4855-b9e8-77ae1b9d49ed</UUID>"
    "<Protocol>NLAP</Protocol>"
    "<Version>0.1</Version>"
    "<Subtype>NLAFP</Subtype>"
    "<Header>"
    "<Host>testapp1.local</Host>"
    "<URL>/testpath/index.html</URL>"
    "<User-Agent>Falcon-Browser</User-Agent>"
    "</Header>"
    "</Request>"
    "</NLAP>"
);

// ---------------------------------------------------------------------------
//  Valid NLAFP Response (single-file, encoded binary)
// ---------------------------------------------------------------------------

static const string NLAFP_RESPONSE_ENCODED_FILE(
    "<NLAP>"
    "<Response>"
    "<UUID>9a728a72-34ac-9abc-2245-af65cbde66ff</UUID>"
    "<Protocol>NLAP</Protocol>"
    "<Version>0.1</Version>"
    "<Subtype>NLAFP</Subtype>"
    "<Header>"
    "<Mime-Type>image/png</Mime-Type>"
    "<Encoding>binary</Encoding>"
    "<Byte-Size>7342</Byte-Size>"
    "<Compression>none</Compression>"
    "</Header>"
    "<Payload>BINARY_PAYLOAD_DATA</Payload>"
    "</Response>"
    "</NLAP>"
);

// ---------------------------------------------------------------------------
//  Valid NLAFP Partial-Transfer Response (first part of multi-part file)
// ---------------------------------------------------------------------------

static const string NLAFP_RESPONSE_PARTIAL_FIRST(
    "<NLAP>"
    "<Response>"
    "<UUID>f3477af2-1212-76af-3377-bc7721afbc7a</UUID>"
    "<Protocol>NLAP</Protocol>"
    "<Version>0.1</Version>"
    "<Subtype>NLAFP</Subtype>"
    "<Header>"
    "<Mime-Type>application/bzip2</Mime-Type>"
    "<Encoding>binary</Encoding>"
    "<Byte-Size-Full>3432132</Byte-Size-Full>"
    "<Byte-Size-Part>100000</Byte-Size-Part>"
    "<Compression>none</Compression>"
    "<File-UUID>f3477af2-1212-76af-3377-bc7721afbc7a</File-UUID>"
    "<File-Part-Sum>35</File-Part-Sum>"
    "<File-Part>1</File-Part>"
    "</Header>"
    "<Payload>PART_ONE_DATA</Payload>"
    "</Response>"
    "</NLAP>"
);

// ---------------------------------------------------------------------------
//  Valid NLAPS Request (encrypted + signed)
// ---------------------------------------------------------------------------

static const string NLAPS_REQUEST_ENCRYPTED(
    "<NLAP>"
    "<Request>"
    "<UUID>a2327a55-33ae-2557-aef2-e42445e5b23a</UUID>"
    "<Protocol>NLAP</Protocol>"
    "<Version>0.1</Version>"
    "<Subtype>NLAPS</Subtype>"
    "<Header>"
    "<Host>testapp2.local</Host>"
    "<URL>/python/service1</URL>"
    "<User-Agent>Falcon-Python-Client</User-Agent>"
    "<user>user1@domain.com</user>"
    "</Header>"
    "<Security>"
    "<Encryption>1</Encryption>"
    "<Signature>BASE64SIGHERE</Signature>"
    "</Security>"
    "<Payload>BASE64ENCRYPTEDPAYLOAD</Payload>"
    "</Request>"
    "</NLAP>"
);

// ---------------------------------------------------------------------------
//  Valid NLAPS Response (encrypted + signed)
// ---------------------------------------------------------------------------

static const string NLAPS_RESPONSE_ENCRYPTED(
    "<NLAP>"
    "<Response>"
    "<UUID>a2327a55-33ae-2557-aef2-e42445e5b23a</UUID>"
    "<Protocol>NLAP</Protocol>"
    "<Version>0.1</Version>"
    "<Subtype>NLAPS</Subtype>"
    "<Header>"
    "<Mime-Type>application/json</Mime-Type>"
    "<Encoding>UTF-8</Encoding>"
    "</Header>"
    "<Security>"
    "<Encryption>1</Encryption>"
    "<Signature>BASE64SIGHERE</Signature>"
    "</Security>"
    "<Payload>BASE64ENCRYPTEDPAYLOAD</Payload>"
    "<Status><Code>0</Code></Status>"
    "</Response>"
    "</NLAP>"
);

// ---------------------------------------------------------------------------
//  Message that already has an XML declaration
// ---------------------------------------------------------------------------

static const string NLAMP_REQUEST_WITH_XMLDECL(
    "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
    "<NLAP>"
    "<Request>"
    "<UUID>decl-uuid-1234</UUID>"
    "<Protocol>NLAP</Protocol>"
    "<Version>0.1</Version>"
    "<Subtype>NLAMP</Subtype>"
    "<Header><Host>declared.local</Host></Header>"
    "</Request>"
    "</NLAP>"
);

// ---------------------------------------------------------------------------
//  INVALID: malformed XML (unclosed tag)
// ---------------------------------------------------------------------------

static const string INVALID_MALFORMED_XML(
    "<NLAP>"
    "<Request>"
    "<UUID>bad-uuid</UUID>"
    "<Protocol>NLAP</Protocol>"
    "<Version>0.1</Version>"
    "<Subtype>NLAMP</Subtype>"
    "<Header><Host>h.local</Host>"   // Header not closed
    "</Request>"
    "</NLAP>"
);

// ---------------------------------------------------------------------------
//  INVALID: missing required UUID element
// ---------------------------------------------------------------------------

static const string INVALID_MISSING_UUID(
    "<NLAP>"
    "<Request>"
    "<Protocol>NLAP</Protocol>"
    "<Version>0.1</Version>"
    "<Subtype>NLAMP</Subtype>"
    "<Header><Host>h.local</Host></Header>"
    "</Request>"
    "</NLAP>"
);

// ---------------------------------------------------------------------------
//  INVALID: missing required Protocol element
// ---------------------------------------------------------------------------

static const string INVALID_MISSING_PROTOCOL(
    "<NLAP>"
    "<Request>"
    "<UUID>some-uuid</UUID>"
    "<Version>0.1</Version>"
    "<Subtype>NLAMP</Subtype>"
    "<Header><Host>h.local</Host></Header>"
    "</Request>"
    "</NLAP>"
);

// ---------------------------------------------------------------------------
//  INVALID: wrong root element (not NLAP)
// ---------------------------------------------------------------------------

static const string INVALID_WRONG_ROOT(
    "<NOTLAP>"
    "<Request>"
    "<UUID>some-uuid</UUID>"
    "<Protocol>NLAP</Protocol>"
    "<Version>0.1</Version>"
    "<Subtype>NLAMP</Subtype>"
    "<Header><Host>h.local</Host></Header>"
    "</Request>"
    "</NOTLAP>"
);

// ---------------------------------------------------------------------------
//  INVALID: unknown element in Request body (not in DTD)
// ---------------------------------------------------------------------------

static const string INVALID_UNKNOWN_ELEMENT(
    "<NLAP>"
    "<Request>"
    "<UUID>some-uuid</UUID>"
    "<Protocol>NLAP</Protocol>"
    "<Version>0.1</Version>"
    "<Subtype>NLAMP</Subtype>"
    "<Header><Host>h.local</Host></Header>"
    "<NotInDTD>some-value</NotInDTD>"
    "</Request>"
    "</NLAP>"
);

// ---------------------------------------------------------------------------
//  INVALID: Security block missing required Signature child
// ---------------------------------------------------------------------------

static const string INVALID_SECURITY_MISSING_SIGNATURE(
    "<NLAP>"
    "<Request>"
    "<UUID>sec-uuid</UUID>"
    "<Protocol>NLAP</Protocol>"
    "<Version>0.1</Version>"
    "<Subtype>NLAPS</Subtype>"
    "<Header><Host>h.local</Host></Header>"
    "<Security>"
    "<Encryption>1</Encryption>"
    "</Security>"
    "</Request>"
    "</NLAP>"
);

// ---------------------------------------------------------------------------
//  INVALID: Security block missing required Encryption child
// ---------------------------------------------------------------------------

static const string INVALID_SECURITY_MISSING_ENCRYPTION(
    "<NLAP>"
    "<Request>"
    "<UUID>sec-uuid</UUID>"
    "<Protocol>NLAP</Protocol>"
    "<Version>0.1</Version>"
    "<Subtype>NLAPS</Subtype>"
    "<Header><Host>h.local</Host></Header>"
    "<Security>"
    "<Signature>BASE64SIG</Signature>"
    "</Security>"
    "</Request>"
    "</NLAP>"
);

// ---------------------------------------------------------------------------
//  INVALID: Status block missing required Code child
// ---------------------------------------------------------------------------

static const string INVALID_STATUS_MISSING_CODE(
    "<NLAP>"
    "<Response>"
    "<UUID>stat-uuid</UUID>"
    "<Protocol>NLAP</Protocol>"
    "<Version>0.1</Version>"
    "<Subtype>NLAMP</Subtype>"
    "<Header><Mime-Type>application/json</Mime-Type></Header>"
    "<Status>"
    "<Description>No Code here</Description>"
    "</Status>"
    "</Response>"
    "</NLAP>"
);

// ---------------------------------------------------------------------------
//  INVALID: Header children in wrong DTD sequence order
//  (Encoding before Mime-Type is allowed per DTD ordering:
//   Host?, URL?, User-Agent?, user?, Connection-Close?, Mime-Type?, Encoding?
//   but Byte-Size before Mime-Type breaks the sequence)
// ---------------------------------------------------------------------------

static const string INVALID_HEADER_WRONG_ORDER(
    "<NLAP>"
    "<Response>"
    "<UUID>order-uuid</UUID>"
    "<Protocol>NLAP</Protocol>"
    "<Version>0.1</Version>"
    "<Subtype>NLAFP</Subtype>"
    "<Header>"
    "<Byte-Size>1234</Byte-Size>"
    "<Mime-Type>image/png</Mime-Type>"    // Mime-Type must come before Byte-Size in DTD
    "</Header>"
    "</Response>"
    "</NLAP>"
);

// ---------------------------------------------------------------------------
//  INVALID: XXE injection attempt via external entity in DOCTYPE
// ---------------------------------------------------------------------------

static const string INVALID_XXE_ATTEMPT(
    "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
    "<!DOCTYPE NLAP ["
    "  <!ENTITY xxe SYSTEM \"file:///etc/passwd\">"
    "]>"
    "<NLAP>"
    "<Request>"
    "<UUID>&xxe;</UUID>"
    "<Protocol>NLAP</Protocol>"
    "<Version>0.1</Version>"
    "<Subtype>NLAMP</Subtype>"
    "<Header><Host>h.local</Host></Header>"
    "</Request>"
    "</NLAP>"
);

// ---------------------------------------------------------------------------
//  INVALID: empty string (no </NLAP> end marker - never triggers processing)
// ---------------------------------------------------------------------------

static const string INVALID_NO_END_MARKER(
    "<NLAP>"
    "<Request>"
    "<UUID>partial-uuid</UUID>"
    "<Protocol>NLAP</Protocol>"
    // message deliberately truncated - no </NLAP>
);


// ===========================================================================
//  TEST CASES - VALID INPUT
// ===========================================================================

BOOST_AUTO_TEST_CASE( test_valid_nlamp_request_full_transmit )
{
    cout << "Valid NLAMP Request (full transmit)." << endl;

    unique_ptr<XMLParser> parser = make_unique<XMLParser>(4096);
    parser->appendBuffer(NLAMP_REQUEST_FULL.c_str(), NLAMP_REQUEST_FULL.length());

    RequestsMap_t requests = parser->getRequests();
    auto rs = requests.size();
    cout << "Parsed count: " << rs << endl;

    BOOST_TEST(rs == 1);

    auto& r = requests.at(0);
    cout << "RequestType=" << r.RequestType << " UUID=" << r.UUID
         << " Protocol=" << r.Protocol << " Version=" << r.Version
         << " Subtype=" << r.Subtype << endl;

    BOOST_TEST(r.RequestType == "Request");
    BOOST_TEST(r.UUID        == "9b327afe-27ae-2367-aef2-e42445e5b23a");
    BOOST_TEST(r.Protocol    == "NLAP");
    BOOST_TEST(r.Version     == "0.1");
    BOOST_TEST(r.Subtype     == "NLAMP");
    BOOST_TEST(r.Header.count("Host")       == 1u);
    BOOST_TEST(r.Header.at("Host")          == "testapp2.local");
    BOOST_TEST(r.Header.count("URL")        == 1u);
    BOOST_TEST(r.Header.at("URL")           == "/python/service1");
    BOOST_TEST(r.Header.count("User-Agent") == 1u);
    BOOST_TEST(r.Header.at("User-Agent")    == "Falcon-Python-Client");
    BOOST_TEST(!r.Payload.empty());
    BOOST_TEST(r.XMLRawMessage.substr(0, 5) == "<?xml");
}

BOOST_AUTO_TEST_CASE( test_valid_nlamp_response_success )
{
    cout << "Valid NLAMP Response success (full transmit)." << endl;

    unique_ptr<XMLParser> parser = make_unique<XMLParser>(4096);
    parser->appendBuffer(NLAMP_RESPONSE_SUCCESS.c_str(), NLAMP_RESPONSE_SUCCESS.length());

    RequestsMap_t requests = parser->getRequests();
    auto rs = requests.size();
    cout << "Parsed count: " << rs << endl;

    BOOST_TEST(rs == 1);
    auto& r = requests.at(0);

    BOOST_TEST(r.RequestType == "Response");
    BOOST_TEST(r.UUID        == "9b327afe-27ae-2367-aef2-e42445e5b23a");
    BOOST_TEST(r.Subtype     == "NLAMP");
    BOOST_TEST(r.Header.at("Mime-Type") == "application/json");
    BOOST_TEST(r.Header.at("Encoding")  == "UTF-8");
    BOOST_TEST(!r.Payload.empty());
    BOOST_TEST(r.StatusCode  == "0");
}

BOOST_AUTO_TEST_CASE( test_valid_nlamp_response_failure_status )
{
    cout << "Valid NLAMP Response failure with full Status block." << endl;

    unique_ptr<XMLParser> parser = make_unique<XMLParser>(4096);
    parser->appendBuffer(NLAMP_RESPONSE_FAILURE.c_str(), NLAMP_RESPONSE_FAILURE.length());

    RequestsMap_t requests = parser->getRequests();
    auto rs = requests.size();
    cout << "Parsed count: " << rs << endl;

    BOOST_TEST(rs == 1);
    auto& r = requests.at(0);

    BOOST_TEST(r.RequestType      == "Response");
    BOOST_TEST(r.Subtype          == "NLAMP");
    BOOST_TEST(r.StatusCode       == "10");
    BOOST_TEST(r.StatusDescription == "Application Exception");
    BOOST_TEST(!r.StatusException.empty());
}

BOOST_AUTO_TEST_CASE( test_valid_nlafp_request_static_file )
{
    cout << "Valid NLAFP Request (static file, no payload)." << endl;

    unique_ptr<XMLParser> parser = make_unique<XMLParser>(4096);
    parser->appendBuffer(NLAFP_REQUEST_STATIC_FILE.c_str(), NLAFP_REQUEST_STATIC_FILE.length());

    RequestsMap_t requests = parser->getRequests();
    auto rs = requests.size();
    cout << "Parsed count: " << rs << endl;

    BOOST_TEST(rs == 1);
    auto& r = requests.at(0);

    BOOST_TEST(r.RequestType == "Request");
    BOOST_TEST(r.UUID        == "7ea45c8a-5193-4855-b9e8-77ae1b9d49ed");
    BOOST_TEST(r.Subtype     == "NLAFP");
    BOOST_TEST(r.Header.at("Host")       == "testapp1.local");
    BOOST_TEST(r.Header.at("URL")        == "/testpath/index.html");
    BOOST_TEST(r.Header.at("User-Agent") == "Falcon-Browser");
    BOOST_TEST(r.Payload.empty());
}

BOOST_AUTO_TEST_CASE( test_valid_nlafp_response_encoded_file )
{
    cout << "Valid NLAFP Response (single encoded file with file headers)." << endl;

    unique_ptr<XMLParser> parser = make_unique<XMLParser>(4096);
    parser->appendBuffer(NLAFP_RESPONSE_ENCODED_FILE.c_str(), NLAFP_RESPONSE_ENCODED_FILE.length());

    RequestsMap_t requests = parser->getRequests();
    auto rs = requests.size();
    cout << "Parsed count: " << rs << endl;

    BOOST_TEST(rs == 1);
    auto& r = requests.at(0);

    BOOST_TEST(r.RequestType         == "Response");
    BOOST_TEST(r.Subtype             == "NLAFP");
    BOOST_TEST(r.Header.at("Mime-Type")   == "image/png");
    BOOST_TEST(r.Header.at("Byte-Size")   == "7342");
    BOOST_TEST(r.Header.at("Compression") == "none");
    BOOST_TEST(r.Header.at("Encoding")    == "binary");
    BOOST_TEST(!r.Payload.empty());
}

BOOST_AUTO_TEST_CASE( test_valid_nlafp_response_partial_transfer )
{
    cout << "Valid NLAFP Partial Transfer Response (multi-part file headers)." << endl;

    unique_ptr<XMLParser> parser = make_unique<XMLParser>(4096);
    parser->appendBuffer(NLAFP_RESPONSE_PARTIAL_FIRST.c_str(), NLAFP_RESPONSE_PARTIAL_FIRST.length());

    RequestsMap_t requests = parser->getRequests();
    auto rs = requests.size();
    cout << "Parsed count: " << rs << endl;

    BOOST_TEST(rs == 1);
    auto& r = requests.at(0);

    BOOST_TEST(r.RequestType == "Response");
    BOOST_TEST(r.Subtype     == "NLAFP");
    BOOST_TEST(r.Header.at("Byte-Size-Full") == "3432132");
    BOOST_TEST(r.Header.at("Byte-Size-Part") == "100000");
    BOOST_TEST(r.Header.at("File-UUID")      == "f3477af2-1212-76af-3377-bc7721afbc7a");
    BOOST_TEST(r.Header.at("File-Part-Sum")  == "35");
    BOOST_TEST(r.Header.at("File-Part")      == "1");
}

BOOST_AUTO_TEST_CASE( test_valid_nlaps_request_encrypted_signed )
{
    cout << "Valid NLAPS Request (encrypted + signed with Security block)." << endl;

    unique_ptr<XMLParser> parser = make_unique<XMLParser>(4096);
    parser->appendBuffer(NLAPS_REQUEST_ENCRYPTED.c_str(), NLAPS_REQUEST_ENCRYPTED.length());

    RequestsMap_t requests = parser->getRequests();
    auto rs = requests.size();
    cout << "Parsed count: " << rs << endl;

    BOOST_TEST(rs == 1);
    auto& r = requests.at(0);

    BOOST_TEST(r.RequestType      == "Request");
    BOOST_TEST(r.Subtype          == "NLAPS");
    BOOST_TEST(r.Header.count("user") == 1u);
    BOOST_TEST(r.Header.at("user")    == "user1@domain.com");
    BOOST_TEST(r.Encryption       == "1");
    BOOST_TEST(r.Signature        == "BASE64SIGHERE");
    BOOST_TEST(!r.Payload.empty());
}

BOOST_AUTO_TEST_CASE( test_valid_nlaps_response_encrypted_signed )
{
    cout << "Valid NLAPS Response (encrypted + signed + Status)." << endl;

    unique_ptr<XMLParser> parser = make_unique<XMLParser>(4096);
    parser->appendBuffer(NLAPS_RESPONSE_ENCRYPTED.c_str(), NLAPS_RESPONSE_ENCRYPTED.length());

    RequestsMap_t requests = parser->getRequests();
    auto rs = requests.size();
    cout << "Parsed count: " << rs << endl;

    BOOST_TEST(rs == 1);
    auto& r = requests.at(0);

    BOOST_TEST(r.RequestType == "Response");
    BOOST_TEST(r.Subtype     == "NLAPS");
    BOOST_TEST(r.Encryption  == "1");
    BOOST_TEST(r.Signature   == "BASE64SIGHERE");
    BOOST_TEST(r.StatusCode  == "0");
}

BOOST_AUTO_TEST_CASE( test_valid_message_with_existing_xml_declaration )
{
    cout << "Valid message that already has an XML declaration." << endl;

    unique_ptr<XMLParser> parser = make_unique<XMLParser>(4096);
    parser->appendBuffer(NLAMP_REQUEST_WITH_XMLDECL.c_str(), NLAMP_REQUEST_WITH_XMLDECL.length());

    RequestsMap_t requests = parser->getRequests();
    auto rs = requests.size();
    cout << "Parsed count: " << rs << endl;

    BOOST_TEST(rs == 1);
    auto& r = requests.at(0);

    BOOST_TEST(r.RequestType == "Request");
    BOOST_TEST(r.UUID        == "decl-uuid-1234");
    BOOST_TEST(r.XMLRawMessage.substr(0, 5) == "<?xml");
}

BOOST_AUTO_TEST_CASE( test_valid_multiple_messages_single_buffer )
{
    cout << "Valid: multiple NLAP messages in one buffer." << endl;

    string combined;
    combined += NLAMP_REQUEST_FULL;
    combined += NLAMP_RESPONSE_SUCCESS;
    combined += NLAFP_REQUEST_STATIC_FILE;

    unique_ptr<XMLParser> parser = make_unique<XMLParser>(8192);
    parser->appendBuffer(combined.c_str(), combined.length());

    RequestsMap_t requests = parser->getRequests();
    auto rs = requests.size();
    cout << "Parsed count: " << rs << endl;

    BOOST_TEST(rs == 3);
    BOOST_TEST(requests.at(0).RequestType == "Request");
    BOOST_TEST(requests.at(0).Subtype     == "NLAMP");
    BOOST_TEST(requests.at(1).RequestType == "Response");
    BOOST_TEST(requests.at(1).Subtype     == "NLAMP");
    BOOST_TEST(requests.at(2).RequestType == "Request");
    BOOST_TEST(requests.at(2).Subtype     == "NLAFP");
}

BOOST_AUTO_TEST_CASE( test_valid_fragmented_delivery_two_parts )
{
    cout << "Valid: NLAMP request split across two appendBuffer calls." << endl;

    const size_t midPoint = NLAMP_REQUEST_FULL.length() / 2;
    const char* data = NLAMP_REQUEST_FULL.c_str();

    unique_ptr<XMLParser> parser = make_unique<XMLParser>(4096);
    parser->appendBuffer(data, static_cast<uint16_t>(midPoint));

    // No complete message yet
    BOOST_TEST(parser->getRequests().size() == 0u);

    parser->appendBuffer(data + midPoint, static_cast<uint16_t>(NLAMP_REQUEST_FULL.length() - midPoint));

    RequestsMap_t requests = parser->getRequests();
    auto rs = requests.size();
    cout << "Parsed count after second part: " << rs << endl;

    BOOST_TEST(rs == 1);
    BOOST_TEST(requests.at(0).RequestType == "Request");
    BOOST_TEST(requests.at(0).UUID        == "9b327afe-27ae-2367-aef2-e42445e5b23a");
}

BOOST_AUTO_TEST_CASE( test_valid_byte_by_byte_delivery )
{
    cout << "Valid: NLAMP request delivered byte by byte." << endl;

    unique_ptr<XMLParser> parser = make_unique<XMLParser>(4096);

    const char* data = NLAMP_REQUEST_FULL.c_str();
    for (size_t i = 0; i < NLAMP_REQUEST_FULL.length(); ++i) {
        parser->appendBuffer(data + i, 1);
    }

    RequestsMap_t requests = parser->getRequests();
    auto rs = requests.size();
    cout << "Parsed count: " << rs << endl;

    BOOST_TEST(rs == 1);
    BOOST_TEST(requests.at(0).RequestType == "Request");
    BOOST_TEST(requests.at(0).UUID        == "9b327afe-27ae-2367-aef2-e42445e5b23a");
}

BOOST_AUTO_TEST_CASE( test_valid_multiple_messages_byte_by_byte )
{
    cout << "Valid: multiple NLAP messages delivered byte by byte." << endl;

    string combined;
    combined += NLAMP_REQUEST_FULL;
    combined += NLAFP_REQUEST_STATIC_FILE;

    unique_ptr<XMLParser> parser = make_unique<XMLParser>(8192);

    const char* data = combined.c_str();
    for (size_t i = 0; i < combined.length(); ++i) {
        parser->appendBuffer(data + i, 1);
    }

    RequestsMap_t requests = parser->getRequests();
    auto rs = requests.size();
    cout << "Parsed count: " << rs << endl;

    BOOST_TEST(rs == 2);
    BOOST_TEST(requests.at(0).Subtype == "NLAMP");
    BOOST_TEST(requests.at(1).Subtype == "NLAFP");
}

BOOST_AUTO_TEST_CASE( test_valid_xmlrawmessage_starts_with_xml_declaration )
{
    cout << "Valid: XMLRawMessage always starts with '<?xml'." << endl;

    unique_ptr<XMLParser> parser = make_unique<XMLParser>(4096);
    parser->appendBuffer(NLAMP_REQUEST_FULL.c_str(), NLAMP_REQUEST_FULL.length());

    RequestsMap_t requests = parser->getRequests();
    BOOST_TEST(requests.size() == 1u);

    const string& raw = requests.at(0).XMLRawMessage;
    cout << "XMLRawMessage prefix: " << raw.substr(0, 20) << endl;

    BOOST_TEST(raw.substr(0, 5) == "<?xml");
}

BOOST_AUTO_TEST_CASE( test_valid_getNextRequest )
{
    cout << "Valid: getNextRequest returns requests in insertion order." << endl;

    string combined;
    combined += NLAMP_REQUEST_FULL;
    combined += NLAMP_RESPONSE_SUCCESS;

    unique_ptr<XMLParser> parser = make_unique<XMLParser>(8192);
    parser->appendBuffer(combined.c_str(), combined.length());

    BOOST_TEST(parser->getRequests().size() == 2u);

    auto r1 = parser->getNextRequest();
    auto r2 = parser->getNextRequest();

    BOOST_TEST(r1 != nullptr);
    BOOST_TEST(r2 != nullptr);

    BOOST_TEST(r1->RequestType == "Request");
    BOOST_TEST(r2->RequestType == "Response");
}

BOOST_AUTO_TEST_CASE( test_valid_removeRequest )
{
    cout << "Valid: removeRequest removes from the map." << endl;

    unique_ptr<XMLParser> parser = make_unique<XMLParser>(4096);
    parser->appendBuffer(NLAMP_REQUEST_FULL.c_str(), NLAMP_REQUEST_FULL.length());

    BOOST_TEST(parser->getRequests().size() == 1u);

    parser->removeRequest(0);

    BOOST_TEST(parser->getRequests().size() == 0u);
}

BOOST_AUTO_TEST_CASE( test_valid_response_status_code_only )
{
    cout << "Valid: Response with Status containing only Code (Description and Exception optional)." << endl;

    // Reuse NLAMP_RESPONSE_SUCCESS which has <Status><Code>0</Code></Status>
    unique_ptr<XMLParser> parser = make_unique<XMLParser>(4096);
    parser->appendBuffer(NLAMP_RESPONSE_SUCCESS.c_str(), NLAMP_RESPONSE_SUCCESS.length());

    RequestsMap_t requests = parser->getRequests();
    BOOST_TEST(requests.size() == 1u);

    auto& r = requests.at(0);
    BOOST_TEST(r.StatusCode        == "0");
    BOOST_TEST(r.StatusDescription == "");
    BOOST_TEST(r.StatusException   == "");
}

BOOST_AUTO_TEST_CASE( test_valid_header_user_field )
{
    cout << "Valid: Header 'user' field (NLAPS user identity) parsed correctly." << endl;

    unique_ptr<XMLParser> parser = make_unique<XMLParser>(4096);
    parser->appendBuffer(NLAPS_REQUEST_ENCRYPTED.c_str(), NLAPS_REQUEST_ENCRYPTED.length());

    RequestsMap_t requests = parser->getRequests();
    BOOST_TEST(requests.size() == 1u);

    BOOST_TEST(requests.at(0).Header.count("user") == 1u);
    BOOST_TEST(requests.at(0).Header.at("user") == "user1@domain.com");
}

// ===========================================================================
//  TEST CASES - INVALID INPUT
// ===========================================================================

BOOST_AUTO_TEST_CASE( test_invalid_no_end_marker )
{
    cout << "Invalid: no end marker </NLAP> - message not processed." << endl;

    unique_ptr<XMLParser> parser = make_unique<XMLParser>(4096);
    parser->appendBuffer(INVALID_NO_END_MARKER.c_str(), INVALID_NO_END_MARKER.length());

    RequestsMap_t requests = parser->getRequests();
    auto rs = requests.size();
    cout << "Parsed count: " << rs << " (expect 0)" << endl;

    BOOST_TEST(rs == 0u);
}

BOOST_AUTO_TEST_CASE( test_invalid_malformed_xml )
{
    cout << "Invalid: malformed XML (unclosed tag) - rejected by parser." << endl;

    unique_ptr<XMLParser> parser = make_unique<XMLParser>(4096);
    parser->appendBuffer(INVALID_MALFORMED_XML.c_str(), INVALID_MALFORMED_XML.length());

    RequestsMap_t requests = parser->getRequests();
    auto rs = requests.size();
    cout << "Parsed count: " << rs << " (expect 0)" << endl;

    BOOST_TEST(rs == 0u);
}

BOOST_AUTO_TEST_CASE( test_invalid_missing_uuid )
{
    cout << "Invalid: missing UUID element - DTD validation rejects." << endl;

    unique_ptr<XMLParser> parser = make_unique<XMLParser>(4096);
    parser->appendBuffer(INVALID_MISSING_UUID.c_str(), INVALID_MISSING_UUID.length());

    RequestsMap_t requests = parser->getRequests();
    auto rs = requests.size();
    cout << "Parsed count: " << rs << " (expect 0)" << endl;

    BOOST_TEST(rs == 0u);
}

BOOST_AUTO_TEST_CASE( test_invalid_missing_protocol )
{
    cout << "Invalid: missing Protocol element - DTD validation rejects." << endl;

    unique_ptr<XMLParser> parser = make_unique<XMLParser>(4096);
    parser->appendBuffer(INVALID_MISSING_PROTOCOL.c_str(), INVALID_MISSING_PROTOCOL.length());

    RequestsMap_t requests = parser->getRequests();
    auto rs = requests.size();
    cout << "Parsed count: " << rs << " (expect 0)" << endl;

    BOOST_TEST(rs == 0u);
}

BOOST_AUTO_TEST_CASE( test_invalid_wrong_root_element )
{
    cout << "Invalid: wrong root element (not NLAP) - DTD validation rejects." << endl;

    unique_ptr<XMLParser> parser = make_unique<XMLParser>(4096);
    // The end marker </NLAP> is not present, so the split on NLAP_XML_END_MARKER
    // will never trigger processing. The message is therefore never parsed.
    string bad = INVALID_WRONG_ROOT;
    parser->appendBuffer(bad.c_str(), bad.length());

    RequestsMap_t requests = parser->getRequests();
    auto rs = requests.size();
    cout << "Parsed count: " << rs << " (expect 0)" << endl;

    BOOST_TEST(rs == 0u);
}

BOOST_AUTO_TEST_CASE( test_invalid_unknown_element_in_request )
{
    cout << "Invalid: unknown element in Request body - DTD validation rejects." << endl;

    unique_ptr<XMLParser> parser = make_unique<XMLParser>(4096);
    parser->appendBuffer(INVALID_UNKNOWN_ELEMENT.c_str(), INVALID_UNKNOWN_ELEMENT.length());

    RequestsMap_t requests = parser->getRequests();
    auto rs = requests.size();
    cout << "Parsed count: " << rs << " (expect 0)" << endl;

    BOOST_TEST(rs == 0u);
}

BOOST_AUTO_TEST_CASE( test_invalid_security_missing_signature )
{
    cout << "Invalid: Security block missing required Signature - DTD rejects." << endl;

    unique_ptr<XMLParser> parser = make_unique<XMLParser>(4096);
    parser->appendBuffer(INVALID_SECURITY_MISSING_SIGNATURE.c_str(),
                         INVALID_SECURITY_MISSING_SIGNATURE.length());

    RequestsMap_t requests = parser->getRequests();
    auto rs = requests.size();
    cout << "Parsed count: " << rs << " (expect 0)" << endl;

    BOOST_TEST(rs == 0u);
}

BOOST_AUTO_TEST_CASE( test_invalid_security_missing_encryption )
{
    cout << "Invalid: Security block missing required Encryption - DTD rejects." << endl;

    unique_ptr<XMLParser> parser = make_unique<XMLParser>(4096);
    parser->appendBuffer(INVALID_SECURITY_MISSING_ENCRYPTION.c_str(),
                         INVALID_SECURITY_MISSING_ENCRYPTION.length());

    RequestsMap_t requests = parser->getRequests();
    auto rs = requests.size();
    cout << "Parsed count: " << rs << " (expect 0)" << endl;

    BOOST_TEST(rs == 0u);
}

BOOST_AUTO_TEST_CASE( test_invalid_status_missing_code )
{
    cout << "Invalid: Status block missing required Code element - DTD rejects." << endl;

    unique_ptr<XMLParser> parser = make_unique<XMLParser>(4096);
    parser->appendBuffer(INVALID_STATUS_MISSING_CODE.c_str(),
                         INVALID_STATUS_MISSING_CODE.length());

    RequestsMap_t requests = parser->getRequests();
    auto rs = requests.size();
    cout << "Parsed count: " << rs << " (expect 0)" << endl;

    BOOST_TEST(rs == 0u);
}

BOOST_AUTO_TEST_CASE( test_invalid_header_wrong_element_order )
{
    cout << "Invalid: Header children in wrong sequence order (Byte-Size before Mime-Type)." << endl;

    unique_ptr<XMLParser> parser = make_unique<XMLParser>(4096);
    parser->appendBuffer(INVALID_HEADER_WRONG_ORDER.c_str(),
                         INVALID_HEADER_WRONG_ORDER.length());

    RequestsMap_t requests = parser->getRequests();
    auto rs = requests.size();
    cout << "Parsed count: " << rs << " (expect 0)" << endl;

    BOOST_TEST(rs == 0u);
}

BOOST_AUTO_TEST_CASE( test_invalid_xxe_injection_attempt )
{
    cout << "Invalid/Security: XXE injection attempt - external entity must not be resolved." << endl;

    unique_ptr<XMLParser> parser = make_unique<XMLParser>(4096);
    parser->appendBuffer(INVALID_XXE_ATTEMPT.c_str(), INVALID_XXE_ATTEMPT.length());

    // XXE must not succeed - message is rejected (entity expanded to empty, UUID becomes "")
    // meaning the message either fails validation or the entity is empty.
    // Either way, /etc/passwd content must NOT appear in any field.
    RequestsMap_t requests = parser->getRequests();
    bool passwdLeaked = false;
    for (auto& kv : requests) {
        if (kv.second.UUID.find("root:") != string::npos) {
            passwdLeaked = true;
        }
    }
    cout << "Passwd leaked: " << (passwdLeaked ? "YES (FAIL)" : "NO (OK)") << endl;
    BOOST_TEST(!passwdLeaked);
}

BOOST_AUTO_TEST_CASE( test_invalid_buffer_overflow )
{
    cout << "Invalid: data exceeds buffer max - appendBuffer silently drops data." << endl;

    // Buffer max = 64 bytes, message is much larger
    unique_ptr<XMLParser> parser = make_unique<XMLParser>(64);
    parser->appendBuffer(NLAMP_REQUEST_FULL.c_str(), NLAMP_REQUEST_FULL.length());

    RequestsMap_t requests = parser->getRequests();
    auto rs = requests.size();
    cout << "Parsed count: " << rs << " (expect 0)" << endl;

    BOOST_TEST(rs == 0u);
}

BOOST_AUTO_TEST_CASE( test_invalid_empty_buffer )
{
    cout << "Invalid: empty buffer - nothing parsed." << endl;

    unique_ptr<XMLParser> parser = make_unique<XMLParser>(4096);
    const char* empty = "";
    parser->appendBuffer(empty, 0);

    RequestsMap_t requests = parser->getRequests();
    auto rs = requests.size();
    cout << "Parsed count: " << rs << " (expect 0)" << endl;

    BOOST_TEST(rs == 0u);
}

BOOST_AUTO_TEST_CASE( test_invalid_mixed_valid_invalid_in_one_buffer )
{
    cout << "Invalid: one valid + one invalid message in same buffer - only valid accepted." << endl;

    string combined;
    combined += NLAMP_REQUEST_FULL;        // valid
    combined += INVALID_MISSING_UUID;      // invalid

    unique_ptr<XMLParser> parser = make_unique<XMLParser>(8192);
    parser->appendBuffer(combined.c_str(), combined.length());

    RequestsMap_t requests = parser->getRequests();
    auto rs = requests.size();
    cout << "Parsed count: " << rs << " (expect 1)" << endl;

    BOOST_TEST(rs == 1u);
    BOOST_TEST(requests.at(0).Subtype == "NLAMP");
}


// ===========================================================================
//  TEST CASES - ZERO-COPY MESSAGE FRAMING (frameMessages)
// ===========================================================================

BOOST_AUTO_TEST_CASE( test_frame_single_message )
{
    cout << "Frame: single complete message." << endl;

    string input = "<NLAP><tag1>value1</tag1></NLAP>";

    auto result = XMLParser::frameMessages(input.c_str(), input.size());

    BOOST_TEST(result.status == FrameStatus::Valid);
    BOOST_TEST(result.message_count == 1u);
    BOOST_TEST(result.messages.size() == 1u);
    BOOST_TEST(result.messages[0].start == input.c_str());
    BOOST_TEST(result.messages[0].length == input.size());
    BOOST_TEST(result.bytes_consumed == input.size());
}

BOOST_AUTO_TEST_CASE( test_frame_two_contiguous_messages )
{
    cout << "Frame: two contiguous messages in one buffer." << endl;

    string msg1 = "<NLAP><tag1>value1</tag1></NLAP>";
    string msg2 = "<NLAP><tag2>value22</tag2></NLAP>";
    string input = msg1 + msg2;

    auto result = XMLParser::frameMessages(input.c_str(), input.size());

    BOOST_TEST(result.status == FrameStatus::Valid);
    BOOST_TEST(result.message_count == 2u);
    BOOST_TEST(result.messages.size() == 2u);

    BOOST_TEST(result.messages[0].start == input.c_str());
    BOOST_TEST(result.messages[0].length == msg1.size());

    BOOST_TEST(result.messages[1].start == input.c_str() + msg1.size());
    BOOST_TEST(result.messages[1].length == msg2.size());

    BOOST_TEST(result.bytes_consumed == input.size());
}

BOOST_AUTO_TEST_CASE( test_frame_three_messages )
{
    cout << "Frame: three contiguous messages." << endl;

    string msg1 = "<NLAP><a>1</a></NLAP>";
    string msg2 = "<NLAP><b>22</b></NLAP>";
    string msg3 = "<NLAP><c>333</c></NLAP>";
    string input = msg1 + msg2 + msg3;

    auto result = XMLParser::frameMessages(input.c_str(), input.size());

    BOOST_TEST(result.status == FrameStatus::Valid);
    BOOST_TEST(result.message_count == 3u);
    BOOST_TEST(result.messages[0].length == msg1.size());
    BOOST_TEST(result.messages[1].length == msg2.size());
    BOOST_TEST(result.messages[2].length == msg3.size());
}

BOOST_AUTO_TEST_CASE( test_frame_string_view_overload )
{
    cout << "Frame: string_view overload works identically." << endl;

    string input = "<NLAP><tag1>value1</tag1></NLAP><NLAP><tag2>value2</tag2></NLAP>";
    string_view sv(input);

    auto result = XMLParser::frameMessages(sv);

    BOOST_TEST(result.status == FrameStatus::Valid);
    BOOST_TEST(result.message_count == 2u);
    BOOST_TEST(result.messages[0].start == input.data());
}

BOOST_AUTO_TEST_CASE( test_frame_incomplete_trailing_message )
{
    cout << "Frame: trailing incomplete message returns Incomplete." << endl;

    string input = "<NLAP><tag1>value1</tag1></NLAP><NLAP><tag2>val";

    auto result = XMLParser::frameMessages(input.c_str(), input.size());

    BOOST_TEST(result.status == FrameStatus::Incomplete);
    BOOST_TEST(result.message_count == 1u);
    BOOST_TEST(result.messages.size() == 1u);
}

BOOST_AUTO_TEST_CASE( test_frame_garbage_before_first_message )
{
    cout << "Frame: garbage before first start tag returns Invalid." << endl;

    string input = "garbage<NLAP><tag1>value1</tag1></NLAP>";

    auto result = XMLParser::frameMessages(input.c_str(), input.size());

    BOOST_TEST(result.status == FrameStatus::Invalid);
}

BOOST_AUTO_TEST_CASE( test_frame_garbage_between_messages )
{
    cout << "Frame: garbage between two messages returns Invalid." << endl;

    string input = "<NLAP><tag1>v1</tag1></NLAP>GARBAGE<NLAP><tag2>v2</tag2></NLAP>";

    auto result = XMLParser::frameMessages(input.c_str(), input.size());

    BOOST_TEST(result.status == FrameStatus::Invalid);
}

BOOST_AUTO_TEST_CASE( test_frame_double_start_tag )
{
    cout << "Frame: double/nested start tag returns Invalid." << endl;

    string input = "<NLAP><NLAP><tag1>value1</tag1></NLAP>";

    auto result = XMLParser::frameMessages(input.c_str(), input.size());

    BOOST_TEST(result.status == FrameStatus::Invalid);
}

BOOST_AUTO_TEST_CASE( test_frame_empty_buffer )
{
    cout << "Frame: empty buffer returns Valid with 0 messages." << endl;

    auto result = XMLParser::frameMessages("", 0);

    BOOST_TEST(result.status == FrameStatus::Valid);
    BOOST_TEST(result.message_count == 0u);
}

BOOST_AUTO_TEST_CASE( test_frame_only_start_tag )
{
    cout << "Frame: only start tag, no end tag returns Incomplete." << endl;

    string input = "<NLAP>";

    auto result = XMLParser::frameMessages(input.c_str(), input.size());

    BOOST_TEST(result.status == FrameStatus::Incomplete);
    BOOST_TEST(result.message_count == 0u);
}

BOOST_AUTO_TEST_CASE( test_frame_pointers_reference_original_buffer )
{
    cout << "Frame: returned pointers reference the original buffer (zero-copy)." << endl;

    string input = "<NLAP><x>1</x></NLAP><NLAP><y>2</y></NLAP>";

    auto result = XMLParser::frameMessages(input.c_str(), input.size());

    BOOST_TEST(result.status == FrameStatus::Valid);
    BOOST_TEST(result.message_count == 2u);

    //- verify that the pointers are into the original buffer, not copies
    BOOST_TEST((const void*)result.messages[0].start >= (const void*)input.c_str());
    BOOST_TEST((const void*)result.messages[0].start < (const void*)(input.c_str() + input.size()));
    BOOST_TEST((const void*)result.messages[1].start >= (const void*)input.c_str());
    BOOST_TEST((const void*)result.messages[1].start < (const void*)(input.c_str() + input.size()));

    //- verify we can construct string_view from the segment
    string_view seg0(result.messages[0].start, result.messages[0].length);
    string_view seg1(result.messages[1].start, result.messages[1].length);
    BOOST_TEST(seg0 == "<NLAP><x>1</x></NLAP>");
    BOOST_TEST(seg1 == "<NLAP><y>2</y></NLAP>");
}

BOOST_AUTO_TEST_CASE( test_frame_multiple_full_nlap_messages )
{
    cout << "Frame: multiple full NLAP protocol messages." << endl;

    string combined;
    combined += NLAMP_REQUEST_FULL;
    combined += NLAMP_RESPONSE_SUCCESS;
    combined += NLAFP_REQUEST_STATIC_FILE;

    auto result = XMLParser::frameMessages(combined.c_str(), combined.size());

    BOOST_TEST(result.status == FrameStatus::Valid);
    BOOST_TEST(result.message_count == 3u);
    BOOST_TEST(result.bytes_consumed == combined.size());
}


// ===========================================================================
//  TEST CASES - ZERO-COPY RECURSIVE TREE PARSING (parseToTree)
// ===========================================================================

BOOST_AUTO_TEST_CASE( test_tree_simple_nlap_request )
{
    cout << "Tree: parse a full NLAMP request into a recursive tree." << endl;

    auto tree = XMLParser::parseToTree(NLAMP_REQUEST_FULL.c_str(), NLAMP_REQUEST_FULL.size());

    //- root should be a branch with one child: "NLAP"
    BOOST_TEST(tree.is_leaf == false);
    BOOST_TEST(tree.children.count("NLAP") == 1u);

    auto& nlap = tree.children.at("NLAP");
    BOOST_TEST(nlap.is_leaf == false);
    BOOST_TEST(nlap.children.count("Request") == 1u);

    auto& req = nlap.children.at("Request");
    BOOST_TEST(req.is_leaf == false);

    //- UUID leaf
    BOOST_TEST(req.children.count("UUID") == 1u);
    auto& uuid = req.children.at("UUID");
    BOOST_TEST(uuid.is_leaf == true);
    BOOST_TEST(uuid.ref.length == 36u);
    BOOST_TEST(string_view(uuid.ref.address, uuid.ref.length) == "9b327afe-27ae-2367-aef2-e42445e5b23a");

    //- Protocol leaf
    BOOST_TEST(req.children.count("Protocol") == 1u);
    BOOST_TEST(string_view(req.children.at("Protocol").ref.address,
                           req.children.at("Protocol").ref.length) == "NLAP");

    //- Header branch
    BOOST_TEST(req.children.count("Header") == 1u);
    auto& hdr = req.children.at("Header");
    BOOST_TEST(hdr.is_leaf == false);

    BOOST_TEST(hdr.children.count("Host") == 1u);
    BOOST_TEST(string_view(hdr.children.at("Host").ref.address,
                           hdr.children.at("Host").ref.length) == "testapp2.local");

    BOOST_TEST(hdr.children.count("URL") == 1u);
    BOOST_TEST(string_view(hdr.children.at("URL").ref.address,
                           hdr.children.at("URL").ref.length) == "/python/service1");

    BOOST_TEST(hdr.children.count("User-Agent") == 1u);
    BOOST_TEST(string_view(hdr.children.at("User-Agent").ref.address,
                           hdr.children.at("User-Agent").ref.length) == "Falcon-Python-Client");
}

BOOST_AUTO_TEST_CASE( test_tree_pointers_reference_original_buffer )
{
    cout << "Tree: all BufferRef pointers reference the original buffer (zero-copy)." << endl;

    auto tree = XMLParser::parseToTree(NLAMP_REQUEST_FULL.c_str(), NLAMP_REQUEST_FULL.size());

    auto& uuid = tree.children.at("NLAP").children.at("Request").children.at("UUID");

    //- verify the pointer is into the original buffer
    BOOST_TEST((const void*)uuid.ref.address >= (const void*)NLAMP_REQUEST_FULL.c_str());
    BOOST_TEST((const void*)uuid.ref.address <
               (const void*)(NLAMP_REQUEST_FULL.c_str() + NLAMP_REQUEST_FULL.size()));
}

BOOST_AUTO_TEST_CASE( test_tree_string_view_overload )
{
    cout << "Tree: string_view overload works identically." << endl;

    string_view sv(NLAMP_REQUEST_FULL);
    auto tree = XMLParser::parseToTree(sv);

    BOOST_TEST(tree.is_leaf == false);
    BOOST_TEST(tree.children.count("NLAP") == 1u);

    auto& uuid = tree.children.at("NLAP").children.at("Request").children.at("UUID");
    BOOST_TEST(uuid.is_leaf == true);
    BOOST_TEST(uuid.ref.length == 36u);
}

BOOST_AUTO_TEST_CASE( test_tree_response_with_status )
{
    cout << "Tree: parse a response with Status block." << endl;

    auto tree = XMLParser::parseToTree(NLAMP_RESPONSE_FAILURE.c_str(), NLAMP_RESPONSE_FAILURE.size());

    auto& resp = tree.children.at("NLAP").children.at("Response");

    //- Status branch
    BOOST_TEST(resp.children.count("Status") == 1u);
    auto& status = resp.children.at("Status");
    BOOST_TEST(status.is_leaf == false);
    BOOST_TEST(status.children.count("Code") == 1u);
    BOOST_TEST(string_view(status.children.at("Code").ref.address,
                           status.children.at("Code").ref.length) == "10");

    BOOST_TEST(status.children.count("Description") == 1u);
    BOOST_TEST(string_view(status.children.at("Description").ref.address,
                           status.children.at("Description").ref.length) == "Application Exception");
}

BOOST_AUTO_TEST_CASE( test_tree_char_array_buffer )
{
    cout << "Tree: operates on a raw char[] buffer (InputBuffer pattern)." << endl;

    //- simulate the InputBuffer pattern from the requirement
    char InputBuffer[4096];
    const char* msg = "<NLAP><Request><UUID>abc-123</UUID><Protocol>NLAP</Protocol></Request></NLAP>";
    size_t msgLen = strlen(msg);
    memcpy(InputBuffer, msg, msgLen);

    string_view InputBufferView(InputBuffer, msgLen);
    auto tree = XMLParser::parseToTree(InputBufferView);

    auto& uuid = tree.children.at("NLAP").children.at("Request").children.at("UUID");
    BOOST_TEST(uuid.is_leaf == true);
    BOOST_TEST(string_view(uuid.ref.address, uuid.ref.length) == "abc-123");

    //- verify pointer is into InputBuffer, not a copy
    BOOST_TEST((const void*)uuid.ref.address >= (const void*)InputBuffer);
    BOOST_TEST((const void*)uuid.ref.address < (const void*)(InputBuffer + 4096));
}

BOOST_AUTO_TEST_CASE( test_tree_empty_buffer )
{
    cout << "Tree: empty buffer returns empty branch node." << endl;

    auto tree = XMLParser::parseToTree("", 0);

    BOOST_TEST(tree.is_leaf == false);
    BOOST_TEST(tree.children.empty() == true);
}

BOOST_AUTO_TEST_CASE( test_tree_payload_as_leaf )
{
    cout << "Tree: Payload text content is captured as a leaf." << endl;

    auto tree = XMLParser::parseToTree(NLAMP_REQUEST_FULL.c_str(), NLAMP_REQUEST_FULL.size());

    auto& payload = tree.children.at("NLAP").children.at("Request").children.at("Payload");
    BOOST_TEST(payload.is_leaf == true);
    BOOST_TEST(payload.ref.length > 0u);
    BOOST_TEST(string_view(payload.ref.address, payload.ref.length)
               == "{\"param1\":\"string1\",\"param2\":\"string2\",\"param3\":100}");
}
