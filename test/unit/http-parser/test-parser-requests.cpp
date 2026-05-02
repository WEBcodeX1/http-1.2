#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>
#include <memory>
#include <iostream>
#include <fstream>
#include <vector>

#include "../../../lib/http/httpparser.hpp"

using namespace std;

static const string HTTP_REQUEST_GET_SINGLE1("GET /test/test.png HTTP/1.1\r\nCustomHeader: one\r\n\r\n");
static const string HTTP_REQUEST_GET_SINGLE2("GET /test/test2.png HTTP/1.1\r\nHost: test.dns\r\nAnotherHeader: two\r\n\r\n");
static const string HTTP_REQUEST_GET_SINGLE3("GET /test/test3.png HTTP/1.1\r\nThirdHeader: three\r\n\r\n");

static const string HTTP_REQUEST_POST_SINGLE("POST /backend/test1 HTTP/1.1\r\nHost: test.loalnet\r\nContent-Type: application/json\r\nContent-Length: 2\r\n\r\n{}");

static const string HTTP_REQUEST_POST_PARTIAL1("POST /other/path HTTP/1.1\r\nHost: test.loal");
static const string HTTP_REQUEST_POST_PARTIAL2("net\r\nContent-Type: application/json\r\nContent-Length: 10\r");
static const string HTTP_REQUEST_POST_PARTIAL3("\n\r\n{12345678}");

static const string HTTP_REQUEST_GET_OK1("GET /test/test.html?a=1&b=2 HTTP/1.1\r\nCustomHeader1: three\r\n\r\n");
static const string HTTP_REQUEST_GET_OK2("GET /test2.html?hello=hello1&test=test2&here=there HTTP/1.1\r\nHeader2: two\r\n\r\n");


BOOST_AUTO_TEST_CASE( test_single_valid_get_request_full_transmit )
{
    cout << "Check single GET request (full buffer transmit at once)." << endl;

    RequestsMap_t Requests;

    unique_ptr<HTTPParser> parser = make_unique<HTTPParser>(4096);

    parser->appendBuffer(HTTP_REQUEST_GET_SINGLE1.c_str(), HTTP_REQUEST_GET_SINGLE1.length());
    Requests = parser->getRequests();

    auto rs = Requests.size();
    auto rv = Requests.at(0).HTTPVersion;
    auto rm = Requests.at(0).HTTPMethod;
    auto ru = Requests.at(0).URL;
    auto rh = Requests.at(0).RequestHeaders.at("CustomHeader");

    cout << "Request size: " << rs << " version: " << rv << " method: " << rm << " URL: " << ru << " Header: " << rh << endl;

    BOOST_TEST(rs == 1);
    BOOST_TEST(rv == 1);
    BOOST_TEST(rm == 1);
    BOOST_TEST(ru == "/test/test.png");
    BOOST_TEST(rh == "one");

}

BOOST_AUTO_TEST_CASE( test_single_post_request_full_transmit )
{
    cout << "Check single POST request (full buffer transmit at once)." << endl;

    RequestsMap_t Requests;

    unique_ptr<HTTPParser> parser = make_unique<HTTPParser>(4096);

    parser->appendBuffer(HTTP_REQUEST_POST_SINGLE.c_str(), HTTP_REQUEST_POST_SINGLE.length());
    Requests = parser->getRequests();

    auto rs = Requests.size();
    auto rv = Requests.at(0).HTTPVersion;
    auto rm = Requests.at(0).HTTPMethod;
    auto ru = Requests.at(0).URL;
    auto rh = Requests.at(0).RequestHeaders.at("Host");
    auto rp = Requests.at(0).Payload;

    cout << "Request size: " << rs << " version: " << rv << " method: " << rm << " URL: " << ru << " Header: " << rh << " Payload: " << rp << endl;

    BOOST_TEST(rs == 1);
    BOOST_TEST(rv == 1);
    BOOST_TEST(rm == 2);
    BOOST_TEST(ru == "/backend/test1");
    BOOST_TEST(rh == "test.loalnet");
    BOOST_TEST(rp == "{}");
}

BOOST_AUTO_TEST_CASE( test_multiple_valid_get_request_full_transmit )
{
    cout << "Check multiple GET requests (full buffer transmit at once)." << endl;

    RequestsMap_t Requests;

    unique_ptr<HTTPParser> parser = make_unique<HTTPParser>(4096);

    string full_request(HTTP_REQUEST_GET_SINGLE1);
    full_request.append(HTTP_REQUEST_GET_SINGLE2);
    full_request.append(HTTP_REQUEST_GET_SINGLE3);

    parser->appendBuffer(full_request.c_str(), full_request.length());
    Requests = parser->getRequests();

    auto rs = Requests.size();
    auto rv1 = Requests.at(0).HTTPVersion;
    auto rm1 = Requests.at(0).HTTPMethod;
    auto ru1 = Requests.at(0).URL;
    auto rh1 = Requests.at(0).RequestHeaders.at("CustomHeader");

    auto rv2 = Requests.at(1).HTTPVersion;
    auto rm2 = Requests.at(1).HTTPMethod;
    auto ru2 = Requests.at(1).URL;
    auto rh2 = Requests.at(1).RequestHeaders.at("AnotherHeader");

    auto rv3 = Requests.at(2).HTTPVersion;
    auto rm3 = Requests.at(2).HTTPMethod;
    auto ru3 = Requests.at(2).URL;
    auto rh3 = Requests.at(2).RequestHeaders.at("ThirdHeader");

    cout << "Request size: " << rs << endl;
    cout << "Request 1 version: " << rv1 << " method: " << rm1 << " URL: " << ru1 << " Header: " << rh1 << endl;
    cout << "Request 2 version: " << rv2 << " method: " << rm2 << " URL: " << ru2 << " Header: " << rh2 << endl;
    cout << "Request 3 version: " << rv3 << " method: " << rm3 << " URL: " << ru3 << " Header: " << rh3 << endl;

    BOOST_TEST(rs == 3);

    BOOST_TEST(rv1 == 1);
    BOOST_TEST(rm1 == 1);
    BOOST_TEST(ru1 == "/test/test.png");
    BOOST_TEST(rh1 == "one");

    BOOST_TEST(rv2 == 1);
    BOOST_TEST(rm2 == 1);
    BOOST_TEST(ru2 == "/test/test2.png");
    BOOST_TEST(rh2 == "two");

    BOOST_TEST(rv3 == 1);
    BOOST_TEST(rm3 == 1);
    BOOST_TEST(ru3 == "/test/test3.png");
    BOOST_TEST(rh3 == "three");

}

BOOST_AUTO_TEST_CASE( test_multiple_valid_get_and_post_requests_full_transmit )
{
    cout << "Check multiple GET and POST requests (full buffer transmit at once)." << endl;

    RequestsMap_t Requests;

    unique_ptr<HTTPParser> parser = make_unique<HTTPParser>(4096);

    string full_request(HTTP_REQUEST_GET_SINGLE1);
    full_request.append(HTTP_REQUEST_POST_SINGLE);
    full_request.append(HTTP_REQUEST_GET_SINGLE2);
    full_request.append(HTTP_REQUEST_POST_SINGLE);
    full_request.append(HTTP_REQUEST_GET_SINGLE3);

    parser->appendBuffer(full_request.c_str(), full_request.length());
    Requests = parser->getRequests();

    auto rs = Requests.size();
    auto rv1 = Requests.at(0).HTTPVersion;
    auto rm1 = Requests.at(0).HTTPMethod;
    auto ru1 = Requests.at(0).URL;
    auto rh1 = Requests.at(0).RequestHeaders.at("CustomHeader");

    auto rv2 = Requests.at(1).HTTPVersion;
    auto rm2 = Requests.at(1).HTTPMethod;
    auto ru2 = Requests.at(1).URL;
    auto rh2 = Requests.at(1).RequestHeaders.at("Content-Type");

    auto rv3 = Requests.at(2).HTTPVersion;
    auto rm3 = Requests.at(2).HTTPMethod;
    auto ru3 = Requests.at(2).URL;
    auto rh3 = Requests.at(2).RequestHeaders.at("Host");

    cout << "Request size: " << rs << endl;
    cout << "Request 1 version: " << rv1 << " method: " << rm1 << " URL: " << ru1 << " Header: " << rh1 << endl;
    cout << "Request 2 version: " << rv2 << " method: " << rm2 << " URL: " << ru2 << " Header: " << rh2 << endl;
    cout << "Request 3 version: " << rv3 << " method: " << rm3 << " URL: " << ru3 << " Header: " << rh3 << endl;

    BOOST_TEST(rs == 5);

    BOOST_TEST(rv1 == 1);
    BOOST_TEST(rm1 == 1);
    BOOST_TEST(ru1 == "/test/test.png");
    BOOST_TEST(rh1 == "one");

    BOOST_TEST(rv2 == 1);
    BOOST_TEST(rm2 == 2);
    BOOST_TEST(ru2 == "/backend/test1");
    BOOST_TEST(rh2 == "application/json");

    BOOST_TEST(rv3 == 1);
    BOOST_TEST(rm3 == 1);
    BOOST_TEST(ru3 == "/test/test2.png");
    BOOST_TEST(rh3 == "test.dns");

}

BOOST_AUTO_TEST_CASE( test_single_valid_post_request_partial_transmit )
{
    cout << "Check single POST request (partial transmit)." << endl;

    RequestsMap_t Requests;

    unique_ptr<HTTPParser> parser = make_unique<HTTPParser>(4096);

    parser->appendBuffer(HTTP_REQUEST_POST_PARTIAL1.c_str(), HTTP_REQUEST_POST_PARTIAL1.length());
    parser->appendBuffer(HTTP_REQUEST_POST_PARTIAL2.c_str(), HTTP_REQUEST_POST_PARTIAL2.length());
    parser->appendBuffer(HTTP_REQUEST_POST_PARTIAL3.c_str(), HTTP_REQUEST_POST_PARTIAL3.length());
    Requests = parser->getRequests();

    auto rs = Requests.size();
    auto rv = Requests.at(0).HTTPVersion;
    auto rm = Requests.at(0).HTTPMethod;
    auto ru = Requests.at(0).URL;
    auto rh = Requests.at(0).RequestHeaders.at("Content-Type");

    cout << "Request size: " << rs << " version: " << rv << " method: " << rm << " URL: " << ru << " Header: " << rh << endl;

    BOOST_TEST(rs == 1);
    BOOST_TEST(rv == 1);
    BOOST_TEST(rm == 2);
    BOOST_TEST(ru == "/other/path");
    BOOST_TEST(rh == "application/json");

}

BOOST_AUTO_TEST_CASE( test_single_valid_get_request_1byte_transmit )
{
    cout << "Check single GET request (1-byte buffer transmit)." << endl;

    RequestsMap_t Requests;

    unique_ptr<HTTPParser> parser = make_unique<HTTPParser>(4096);

    // send data byte by byte
    for (size_t i = 0; i < HTTP_REQUEST_GET_SINGLE1.length(); i++) {
        parser->appendBuffer(HTTP_REQUEST_GET_SINGLE1.c_str() + i, 1);
    }

    Requests = parser->getRequests();

    auto rs = Requests.size();
    auto rv = Requests.at(0).HTTPVersion;
    auto rm = Requests.at(0).HTTPMethod;
    auto ru = Requests.at(0).URL;
    auto rh = Requests.at(0).RequestHeaders.at("CustomHeader");

    cout << "Request size: " << rs << " version: " << rv << " method: " << rm << " URL: " << ru << " Header: " << rh << endl;

    BOOST_TEST(rs == 1);
    BOOST_TEST(rv == 1);
    BOOST_TEST(rm == 1);
    BOOST_TEST(ru == "/test/test.png");
    BOOST_TEST(rh == "one");
}

BOOST_AUTO_TEST_CASE( test_single_post_request_1byte_transmit )
{
    cout << "Check single POST request (1-byte buffer transmit)." << endl;

    RequestsMap_t Requests;

    unique_ptr<HTTPParser> parser = make_unique<HTTPParser>(4096);

    // send data byte by byte
    for (size_t i = 0; i < HTTP_REQUEST_POST_SINGLE.length(); i++) {
        parser->appendBuffer(HTTP_REQUEST_POST_SINGLE.c_str() + i, 1);
    }

    Requests = parser->getRequests();

    auto rs = Requests.size();
    auto rv = Requests.at(0).HTTPVersion;
    auto rm = Requests.at(0).HTTPMethod;
    auto ru = Requests.at(0).URL;
    auto rh = Requests.at(0).RequestHeaders.at("Host");
    auto rp = Requests.at(0).Payload;

    cout << "Request size: " << rs << " version: " << rv << " method: " << rm << " URL: " << ru << " Header: " << rh << " Payload: " << rp << endl;

    BOOST_TEST(rs == 1);
    BOOST_TEST(rv == 1);
    BOOST_TEST(rm == 2);
    BOOST_TEST(ru == "/backend/test1");
    BOOST_TEST(rh == "test.loalnet");
    BOOST_TEST(rp == "{}");
}

BOOST_AUTO_TEST_CASE( test_multiple_valid_get_request_1byte_transmit )
{
    cout << "Check multiple GET requests (1-byte buffer transmit)." << endl;

    RequestsMap_t Requests;

    unique_ptr<HTTPParser> parser = make_unique<HTTPParser>(4096);

    string full_request(HTTP_REQUEST_GET_SINGLE1);
    full_request.append(HTTP_REQUEST_GET_SINGLE2);
    full_request.append(HTTP_REQUEST_GET_SINGLE3);

    // send data byte by byte
    for (size_t i = 0; i < full_request.length(); i++) {
        parser->appendBuffer(full_request.c_str() + i, 1);
    }

    Requests = parser->getRequests();

    auto rs = Requests.size();
    auto rv1 = Requests.at(0).HTTPVersion;
    auto rm1 = Requests.at(0).HTTPMethod;
    auto ru1 = Requests.at(0).URL;
    auto rh1 = Requests.at(0).RequestHeaders.at("CustomHeader");

    auto rv2 = Requests.at(1).HTTPVersion;
    auto rm2 = Requests.at(1).HTTPMethod;
    auto ru2 = Requests.at(1).URL;
    auto rh2 = Requests.at(1).RequestHeaders.at("AnotherHeader");

    auto rv3 = Requests.at(2).HTTPVersion;
    auto rm3 = Requests.at(2).HTTPMethod;
    auto ru3 = Requests.at(2).URL;
    auto rh3 = Requests.at(2).RequestHeaders.at("ThirdHeader");

    cout << "Request size: " << rs << endl;
    cout << "Request 1 version: " << rv1 << " method: " << rm1 << " URL: " << ru1 << " Header: " << rh1 << endl;
    cout << "Request 2 version: " << rv2 << " method: " << rm2 << " URL: " << ru2 << " Header: " << rh2 << endl;
    cout << "Request 3 version: " << rv3 << " method: " << rm3 << " URL: " << ru3 << " Header: " << rh3 << endl;

    BOOST_TEST(rs == 3);
    BOOST_TEST(rv1 == 1);
    BOOST_TEST(rm1 == 1);
    BOOST_TEST(ru1 == "/test/test.png");
    BOOST_TEST(rh1 == "one");

    BOOST_TEST(rv2 == 1);
    BOOST_TEST(rm2 == 1);
    BOOST_TEST(ru2 == "/test/test2.png");
    BOOST_TEST(rh2 == "two");

    BOOST_TEST(rv3 == 1);
    BOOST_TEST(rm3 == 1);
    BOOST_TEST(ru3 == "/test/test3.png");
    BOOST_TEST(rh3 == "three");
}

BOOST_AUTO_TEST_CASE( test_multiple_valid_get_and_post_requests_1byte_transmit )
{
    cout << "Check multiple GET and POST requests (1-byte buffer transmit)." << endl;

    RequestsMap_t Requests;

    unique_ptr<HTTPParser> parser = make_unique<HTTPParser>(4096);

    string full_request(HTTP_REQUEST_GET_SINGLE1);
    full_request.append(HTTP_REQUEST_POST_SINGLE);
    full_request.append(HTTP_REQUEST_GET_SINGLE2);
    full_request.append(HTTP_REQUEST_POST_SINGLE);
    full_request.append(HTTP_REQUEST_GET_SINGLE3);

    // send data byte by byte
    for (size_t i = 0; i < full_request.length(); i++) {
        parser->appendBuffer(full_request.c_str() + i, 1);
    }

    Requests = parser->getRequests();

    auto rs = Requests.size();
    auto rv1 = Requests.at(0).HTTPVersion;
    auto rm1 = Requests.at(0).HTTPMethod;
    auto ru1 = Requests.at(0).URL;
    auto rh1 = Requests.at(0).RequestHeaders.at("CustomHeader");

    auto rv2 = Requests.at(1).HTTPVersion;
    auto rm2 = Requests.at(1).HTTPMethod;
    auto ru2 = Requests.at(1).URL;
    auto rh2 = Requests.at(1).RequestHeaders.at("Content-Type");

    auto rv3 = Requests.at(2).HTTPVersion;
    auto rm3 = Requests.at(2).HTTPMethod;
    auto ru3 = Requests.at(2).URL;
    auto rh3 = Requests.at(2).RequestHeaders.at("Host");

    cout << "Request size: " << rs << endl;
    cout << "Request 1 version: " << rv1 << " method: " << rm1 << " URL: " << ru1 << " Header: " << rh1 << endl;
    cout << "Request 2 version: " << rv2 << " method: " << rm2 << " URL: " << ru2 << " Header: " << rh2 << endl;
    cout << "Request 3 version: " << rv3 << " method: " << rm3 << " URL: " << ru3 << " Header: " << rh3 << endl;

    BOOST_TEST(rs == 5);
    BOOST_TEST(rv1 == 1);
    BOOST_TEST(rm1 == 1);
    BOOST_TEST(ru1 == "/test/test.png");
    BOOST_TEST(rh1 == "one");

    BOOST_TEST(rv2 == 1);
    BOOST_TEST(rm2 == 2);
    BOOST_TEST(ru2 == "/backend/test1");
    BOOST_TEST(rh2 == "application/json");

    BOOST_TEST(rv3 == 1);
    BOOST_TEST(rm3 == 1);
    BOOST_TEST(ru3 == "/test/test2.png");
    BOOST_TEST(rh3 == "test.dns");
}

BOOST_AUTO_TEST_CASE( test_single_valid_post_request_partial_1byte_transmit )
{
    cout << "Check single POST request (partial 1-byte buffer transmit)." << endl;
    RequestsMap_t Requests;

    unique_ptr<HTTPParser> parser = make_unique<HTTPParser>(4096);

    // send each partial byte by byte
    for (size_t i = 0; i < HTTP_REQUEST_POST_PARTIAL1.length(); i++) {
        parser->appendBuffer(HTTP_REQUEST_POST_PARTIAL1.c_str() + i, 1);
    }
    for (size_t i = 0; i < HTTP_REQUEST_POST_PARTIAL2.length(); i++) {
        parser->appendBuffer(HTTP_REQUEST_POST_PARTIAL2.c_str() + i, 1);
    }
    for (size_t i = 0; i < HTTP_REQUEST_POST_PARTIAL3.length(); i++) {
        parser->appendBuffer(HTTP_REQUEST_POST_PARTIAL3.c_str() + i, 1);
    }

    Requests = parser->getRequests();

    auto rs = Requests.size();
    auto rv = Requests.at(0).HTTPVersion;
    auto rm = Requests.at(0).HTTPMethod;
    auto ru = Requests.at(0).URL;
    auto rh = Requests.at(0).RequestHeaders.at("Content-Type");

    cout << "Request size: " << rs << " version: " << rv << " method: " << rm << " URL: " << ru << " Header: " << rh << endl;

    BOOST_TEST(rs == 1);
    BOOST_TEST(rv == 1);
    BOOST_TEST(rm == 2);
    BOOST_TEST(ru == "/other/path");
    BOOST_TEST(rh == "application/json");
}

BOOST_AUTO_TEST_CASE( test_valid_get_parameters_1 )
{
    cout << "Check valid GET parameters request 1." << endl;

    RequestsMap_t Requests;

    unique_ptr<HTTPParser> parser = make_unique<HTTPParser>(4096);

    parser->appendBuffer(HTTP_REQUEST_GET_OK1.c_str(), HTTP_REQUEST_GET_OK1.length());
    Requests = parser->getRequests();

    auto rs = Requests.size();
    auto rv = Requests.at(0).HTTPVersion;
    auto rm = Requests.at(0).HTTPMethod;
    auto ru = Requests.at(0).URL;
    auto rh = Requests.at(0).RequestHeaders.at("CustomHeader1");
    auto rg1 = Requests.at(0).URLParams.at("a");
    auto rg2 = Requests.at(0).URLParams.at("b");

    cout << "Request size: " << rs << " version: " << rv << " method: " << rm << " URL: " << ru << " Header: " << rh << endl;

    BOOST_TEST(rs == 1);
    BOOST_TEST(rv == 1);
    BOOST_TEST(rm == 1);
    BOOST_TEST(ru == "/test/test.html?a=1&b=2");
    BOOST_TEST(rh == "three");
    BOOST_TEST(rg1 == "1");
    BOOST_TEST(rg2 == "2");

}

BOOST_AUTO_TEST_CASE( test_valid_get_parameters_2 )
{
    cout << "Check valid GET parameters request 2." << endl;

    RequestsMap_t Requests;

    unique_ptr<HTTPParser> parser = make_unique<HTTPParser>(4096);

    parser->appendBuffer(HTTP_REQUEST_GET_OK2.c_str(), HTTP_REQUEST_GET_OK2.length());
    Requests = parser->getRequests();

    auto rs = Requests.size();
    auto rv = Requests.at(0).HTTPVersion;
    auto rm = Requests.at(0).HTTPMethod;
    auto ru = Requests.at(0).URL;
    auto rh = Requests.at(0).RequestHeaders.at("Header2");
    auto rg1 = Requests.at(0).URLParams.at("hello");
    auto rg2 = Requests.at(0).URLParams.at("test");
    auto rg3 = Requests.at(0).URLParams.at("here");

    cout << "Request size: " << rs << " version: " << rv << " method: " << rm << " URL: " << ru << " Header: " << rh << endl;

    BOOST_TEST(rs == 1);
    BOOST_TEST(rv == 1);
    BOOST_TEST(rm == 1);
    BOOST_TEST(ru == "/test2.html?hello=hello1&test=test2&here=there");
    BOOST_TEST(rh == "two");
    BOOST_TEST(rg1 == "hello1");
    BOOST_TEST(rg2 == "test2");
    BOOST_TEST(rg3 == "there");

}

BOOST_AUTO_TEST_CASE( test_urlparamsmap2_json )
{
    cout << "Check urlparamsmap to JSON conversion." << endl;

    RequestsMap_t Requests;

    unique_ptr<HTTPParser> parser = make_unique<HTTPParser>(4096);

    parser->appendBuffer(HTTP_REQUEST_GET_OK2.c_str(), HTTP_REQUEST_GET_OK2.length());
    Requests = parser->getRequests();

    auto rs = Requests.size();
    auto rv = Requests.at(0).HTTPVersion;
    auto rm = Requests.at(0).HTTPMethod;
    auto ru = Requests.at(0).URL;
    auto rh = Requests.at(0).RequestHeaders.at("Header2");
    auto rg1 = Requests.at(0).URLParams.at("hello");
    auto rg2 = Requests.at(0).URLParams.at("test");
    auto rg3 = Requests.at(0).URLParams.at("here");

    cout << "Request size: " << rs << " version: " << rv << " method: " << rm << " URL: " << ru << " Header: " << rh << endl;

    BOOST_TEST(rs == 1);
    BOOST_TEST(rv == 1);
    BOOST_TEST(rm == 1);
    BOOST_TEST(ru == "/test2.html?hello=hello1&test=test2&here=there");
    BOOST_TEST(rh == "two");
    BOOST_TEST(rg1 == "hello1");
    BOOST_TEST(rg2 == "test2");
    BOOST_TEST(rg3 == "there");

    string JSONResult;
    JSON::URLParamsMap2JSON(Requests.at(0).URLParams, JSONResult);

    cout << "JSON: " << JSONResult << endl;

    BOOST_TEST(JSONResult == "{ \"payload\": { \"here\": \"there\", \"test\": \"test2\", \"hello\": \"hello1\" }}");
}


// =========================================================================
// Category 1: Malformed headers (missing space or disallowed characters)
// =========================================================================

BOOST_AUTO_TEST_CASE( test_malformed_header_missing_space_after_colon )
{
    cout << "Check malformed header: missing space after colon (Host:example.com)." << endl;

    // Header uses ":" without the required space: "Host:example.com" instead of "Host: example.com"
    const string bad_request("GET /test HTTP/1.1\r\nHost:example.com\r\nContent-Type: text/html\r\n\r\n");

    unique_ptr<HTTPParser> parser = make_unique<HTTPParser>(4096);
    BOOST_CHECK_NO_THROW(parser->appendBuffer(bad_request.c_str(), bad_request.length()));
    RequestsMap_t Requests = parser->getRequests();

    cout << "Requests parsed: " << Requests.size() << endl;

    // The request line is valid; the malformed header is silently skipped
    BOOST_TEST(Requests.size() == 1);
    // "Host" must NOT appear in headers (the ": " delimiter was not found)
    BOOST_TEST(Requests.at(0).RequestHeaders.count("Host") == 0);
    // The well-formed header IS present
    BOOST_TEST(Requests.at(0).RequestHeaders.count("Content-Type") == 1);
}

BOOST_AUTO_TEST_CASE( test_malformed_header_disallowed_at_character )
{
    cout << "Check malformed header: @ character in header name (H@ost: value)." << endl;

    const string bad_request("GET /test HTTP/1.1\r\nH@ost: example.com\r\nValid-Header: ok\r\n\r\n");

    unique_ptr<HTTPParser> parser = make_unique<HTTPParser>(4096);
    BOOST_CHECK_NO_THROW(parser->appendBuffer(bad_request.c_str(), bad_request.length()));
    RequestsMap_t Requests = parser->getRequests();

    cout << "Requests parsed: " << Requests.size() << endl;

    // Parser must not crash; request line is valid so 1 request is parsed
    BOOST_TEST(Requests.size() == 1);
    BOOST_TEST(Requests.at(0).URL == "/test");
    // Valid header must still be present
    BOOST_TEST(Requests.at(0).RequestHeaders.count("Valid-Header") == 1);
}

BOOST_AUTO_TEST_CASE( test_malformed_header_empty_name )
{
    cout << "Check malformed header: empty header name (': value')." << endl;

    // Header starting with ": " has an empty name
    const string bad_request("GET /test HTTP/1.1\r\n: emptyname\r\nHost: valid.host\r\n\r\n");

    unique_ptr<HTTPParser> parser = make_unique<HTTPParser>(4096);
    BOOST_CHECK_NO_THROW(parser->appendBuffer(bad_request.c_str(), bad_request.length()));
    RequestsMap_t Requests = parser->getRequests();

    cout << "Requests parsed: " << Requests.size() << endl;

    // Parser must not crash or infinite-loop; the empty-name header is skipped
    BOOST_TEST(Requests.size() == 1);
    BOOST_TEST(Requests.at(0).RequestHeaders.count("") == 0);
    BOOST_TEST(Requests.at(0).RequestHeaders.count("Host") == 1);
}

BOOST_AUTO_TEST_CASE( test_malformed_header_multiple_combined )
{
    cout << "Check multiple malformed headers combined." << endl;

    const string bad_request(
        "GET /path HTTP/1.1\r\n"
        "Host:nospace\r\n"
        "X-Bad@Name: badval\r\n"
        ": emptyname\r\n"
        "ValidHeader: validvalue\r\n"
        "Another@Bad: another\r\n"
        "\r\n"
    );

    unique_ptr<HTTPParser> parser = make_unique<HTTPParser>(4096);
    BOOST_CHECK_NO_THROW(parser->appendBuffer(bad_request.c_str(), bad_request.length()));
    RequestsMap_t Requests = parser->getRequests();

    cout << "Requests parsed: " << Requests.size() << endl;

    // Parser must not crash; valid request line means 1 request is parsed
    BOOST_TEST(Requests.size() == 1);
    // Only the correctly formatted header must be present
    BOOST_TEST(Requests.at(0).RequestHeaders.count("ValidHeader") == 1);
    BOOST_TEST(Requests.at(0).RequestHeaders.at("ValidHeader") == "validvalue");
    // Malformed headers must NOT appear as "Host" (no-space) or empty name
    BOOST_TEST(Requests.at(0).RequestHeaders.count("Host") == 0);
    BOOST_TEST(Requests.at(0).RequestHeaders.count("") == 0);
}

BOOST_AUTO_TEST_CASE( test_malformed_header_null_byte_in_value )
{
    cout << "Check malformed header: null byte embedded in header value." << endl;

    string bad_request("GET /test HTTP/1.1\r\nHost: exam");
    bad_request.push_back('\0');
    bad_request += "ple.com\r\nX-Other: present\r\n\r\n";

    unique_ptr<HTTPParser> parser = make_unique<HTTPParser>(4096);
    BOOST_CHECK_NO_THROW(parser->appendBuffer(bad_request.c_str(), bad_request.length()));
    RequestsMap_t Requests = parser->getRequests();

    cout << "Requests parsed: " << Requests.size() << endl;

    // Must not crash; request line is valid
    BOOST_TEST(Requests.size() == 1);
}

BOOST_AUTO_TEST_CASE( test_malformed_header_only_colon )
{
    cout << "Check malformed header: header that is only a colon (':')." << endl;

    const string bad_request("GET /test HTTP/1.1\r\n:\r\nHost: example.com\r\n\r\n");

    unique_ptr<HTTPParser> parser = make_unique<HTTPParser>(4096);
    BOOST_CHECK_NO_THROW(parser->appendBuffer(bad_request.c_str(), bad_request.length()));
    RequestsMap_t Requests = parser->getRequests();

    cout << "Requests parsed: " << Requests.size() << endl;

    BOOST_TEST(Requests.size() == 1);
    BOOST_TEST(Requests.at(0).RequestHeaders.count("") == 0);
}


// =========================================================================
// Category 2: Non-GET/POST HTTP methods
// =========================================================================

BOOST_AUTO_TEST_CASE( test_invalid_method_delete )
{
    cout << "Check non-GET/POST method: DELETE." << endl;

    const string request("DELETE /test HTTP/1.1\r\nHost: example.com\r\n\r\n");

    unique_ptr<HTTPParser> parser = make_unique<HTTPParser>(4096);
    BOOST_CHECK_NO_THROW(parser->appendBuffer(request.c_str(), request.length()));
    RequestsMap_t Requests = parser->getRequests();

    cout << "Requests parsed: " << Requests.size() << endl;
    BOOST_TEST(Requests.size() == 0);
}

BOOST_AUTO_TEST_CASE( test_invalid_method_put )
{
    cout << "Check non-GET/POST method: PUT." << endl;

    const string request("PUT /resource HTTP/1.1\r\nHost: example.com\r\nContent-Length: 0\r\n\r\n");

    unique_ptr<HTTPParser> parser = make_unique<HTTPParser>(4096);
    BOOST_CHECK_NO_THROW(parser->appendBuffer(request.c_str(), request.length()));
    RequestsMap_t Requests = parser->getRequests();

    cout << "Requests parsed: " << Requests.size() << endl;
    BOOST_TEST(Requests.size() == 0);
}

BOOST_AUTO_TEST_CASE( test_invalid_method_patch )
{
    cout << "Check non-GET/POST method: PATCH." << endl;

    const string request("PATCH /resource HTTP/1.1\r\nHost: example.com\r\nContent-Length: 5\r\n\r\nhello");

    unique_ptr<HTTPParser> parser = make_unique<HTTPParser>(4096);
    BOOST_CHECK_NO_THROW(parser->appendBuffer(request.c_str(), request.length()));
    RequestsMap_t Requests = parser->getRequests();

    cout << "Requests parsed: " << Requests.size() << endl;
    BOOST_TEST(Requests.size() == 0);
}

BOOST_AUTO_TEST_CASE( test_invalid_method_head )
{
    cout << "Check non-GET/POST method: HEAD." << endl;

    const string request("HEAD /test HTTP/1.1\r\nHost: example.com\r\n\r\n");

    unique_ptr<HTTPParser> parser = make_unique<HTTPParser>(4096);
    BOOST_CHECK_NO_THROW(parser->appendBuffer(request.c_str(), request.length()));
    RequestsMap_t Requests = parser->getRequests();

    cout << "Requests parsed: " << Requests.size() << endl;
    BOOST_TEST(Requests.size() == 0);
}

BOOST_AUTO_TEST_CASE( test_invalid_method_options )
{
    cout << "Check non-GET/POST method: OPTIONS." << endl;

    const string request("OPTIONS * HTTP/1.1\r\nHost: example.com\r\n\r\n");

    unique_ptr<HTTPParser> parser = make_unique<HTTPParser>(4096);
    BOOST_CHECK_NO_THROW(parser->appendBuffer(request.c_str(), request.length()));
    RequestsMap_t Requests = parser->getRequests();

    cout << "Requests parsed: " << Requests.size() << endl;
    BOOST_TEST(Requests.size() == 0);
}

BOOST_AUTO_TEST_CASE( test_invalid_method_connect )
{
    cout << "Check non-GET/POST method: CONNECT." << endl;

    const string request("CONNECT example.com:443 HTTP/1.1\r\nHost: example.com:443\r\n\r\n");

    unique_ptr<HTTPParser> parser = make_unique<HTTPParser>(4096);
    BOOST_CHECK_NO_THROW(parser->appendBuffer(request.c_str(), request.length()));
    RequestsMap_t Requests = parser->getRequests();

    cout << "Requests parsed: " << Requests.size() << endl;
    BOOST_TEST(Requests.size() == 0);
}

BOOST_AUTO_TEST_CASE( test_invalid_method_trace )
{
    cout << "Check non-GET/POST method: TRACE." << endl;

    const string request("TRACE /test HTTP/1.1\r\nHost: example.com\r\n\r\n");

    unique_ptr<HTTPParser> parser = make_unique<HTTPParser>(4096);
    BOOST_CHECK_NO_THROW(parser->appendBuffer(request.c_str(), request.length()));
    RequestsMap_t Requests = parser->getRequests();

    cout << "Requests parsed: " << Requests.size() << endl;
    BOOST_TEST(Requests.size() == 0);
}


// =========================================================================
// Category 3: Random garbage bytes
// =========================================================================

BOOST_AUTO_TEST_CASE( test_garbage_printable_ascii )
{
    cout << "Check garbage: printable ASCII non-HTTP data." << endl;

    const string garbage("ThisisNotHTTP RandomGarbage!@#$%^&*()~`<>?/\\|");

    unique_ptr<HTTPParser> parser = make_unique<HTTPParser>(4096);
    BOOST_CHECK_NO_THROW(parser->appendBuffer(garbage.c_str(), garbage.length()));
    RequestsMap_t Requests = parser->getRequests();

    cout << "Requests parsed: " << Requests.size() << endl;
    BOOST_TEST(Requests.size() == 0);
}

BOOST_AUTO_TEST_CASE( test_garbage_raw_binary_bytes )
{
    cout << "Check garbage: raw binary bytes (non-printable)." << endl;

    const unsigned char garbage[] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
        0xFF, 0xFE, 0xFD, 0xFC, 0xFB, 0xFA, 0xF9, 0xF8,
        0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
        0xAB, 0xCD, 0xEF, 0x12, 0x34, 0x56, 0x78, 0x9A
    };

    unique_ptr<HTTPParser> parser = make_unique<HTTPParser>(4096);
    BOOST_CHECK_NO_THROW(
        parser->appendBuffer(reinterpret_cast<const char*>(garbage), sizeof(garbage))
    );
    RequestsMap_t Requests = parser->getRequests();

    cout << "Requests parsed: " << Requests.size() << endl;
    BOOST_TEST(Requests.size() == 0);
}

BOOST_AUTO_TEST_CASE( test_garbage_with_embedded_end_marker )
{
    cout << "Check garbage: binary data containing embedded HTTP end marker (CRLFCRLF)." << endl;

    // Include the HTTP end marker to force a parse attempt on the preceding garbage
    const unsigned char garbage[] = {
        0xDE, 0xAD, 0xBE, 0xEF, 0xCA, 0xFE, 0xBA, 0xBE,
        0x0D, 0x0A, 0x0D, 0x0A,   // \r\n\r\n triggers parse attempt
        0xFF, 0xFE, 0xAB, 0xCD, 0x12, 0x34, 0x56, 0x78
    };

    unique_ptr<HTTPParser> parser = make_unique<HTTPParser>(4096);
    BOOST_CHECK_NO_THROW(
        parser->appendBuffer(reinterpret_cast<const char*>(garbage), sizeof(garbage))
    );
    RequestsMap_t Requests = parser->getRequests();

    cout << "Requests parsed: " << Requests.size() << endl;
    // Garbage before the end marker cannot form a valid HTTP request
    BOOST_TEST(Requests.size() == 0);
}

BOOST_AUTO_TEST_CASE( test_garbage_with_fake_http_method_prefix )
{
    cout << "Check garbage: data starting with 'GET' but otherwise malformed." << endl;

    // Starts with "GET" but has no valid URL, version, or CRLF structure
    const string garbage("GET#@$!%^&*garbage_not_http_at_all\r\n\r\n");

    unique_ptr<HTTPParser> parser = make_unique<HTTPParser>(4096);
    BOOST_CHECK_NO_THROW(parser->appendBuffer(garbage.c_str(), garbage.length()));
    RequestsMap_t Requests = parser->getRequests();

    cout << "Requests parsed: " << Requests.size() << endl;
    BOOST_TEST(Requests.size() == 0);
}

BOOST_AUTO_TEST_CASE( test_garbage_empty_input )
{
    cout << "Check garbage: completely empty input." << endl;

    unique_ptr<HTTPParser> parser = make_unique<HTTPParser>(4096);
    BOOST_CHECK_NO_THROW(parser->appendBuffer("", 0));
    RequestsMap_t Requests = parser->getRequests();

    cout << "Requests parsed: " << Requests.size() << endl;
    BOOST_TEST(Requests.size() == 0);
}

BOOST_AUTO_TEST_CASE( test_garbage_only_end_marker )
{
    cout << "Check garbage: only the HTTP end marker (CRLFCRLF), no request." << endl;

    const string only_end_marker("\r\n\r\n");

    unique_ptr<HTTPParser> parser = make_unique<HTTPParser>(4096);
    BOOST_CHECK_NO_THROW(parser->appendBuffer(only_end_marker.c_str(), only_end_marker.length()));
    RequestsMap_t Requests = parser->getRequests();

    cout << "Requests parsed: " << Requests.size() << endl;
    BOOST_TEST(Requests.size() == 0);
}

BOOST_AUTO_TEST_CASE( test_garbage_repeated_end_markers )
{
    cout << "Check garbage: repeated HTTP end markers with no request data." << endl;

    const string garbage("\r\n\r\n\r\n\r\n\r\n\r\n");

    unique_ptr<HTTPParser> parser = make_unique<HTTPParser>(4096);
    BOOST_CHECK_NO_THROW(parser->appendBuffer(garbage.c_str(), garbage.length()));
    RequestsMap_t Requests = parser->getRequests();

    cout << "Requests parsed: " << Requests.size() << endl;
    BOOST_TEST(Requests.size() == 0);
}


// =========================================================================
// Category 4: Binary file input (/bin/kmod)
// =========================================================================

// Synthetic ELF-like binary data used as a fallback when /bin/kmod is absent.
// An ELF header starts with the 4-byte magic 0x7F 'E' 'L' 'F'; the rest of
// these bytes fill a realistic 64-byte blob that cannot form valid HTTP/1.1.
static const unsigned char SYNTHETIC_BINARY_BLOB[] = {
    0x7F, 0x45, 0x4C, 0x46, 0x02, 0x01, 0x01, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x02, 0x00, 0x3E, 0x00, 0x01, 0x00, 0x00, 0x00,
    0xB0, 0x10, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xF8, 0xC5, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x38, 0x00,
    0x09, 0x00, 0x40, 0x00, 0x1E, 0x00, 0x1D, 0x00
};

BOOST_AUTO_TEST_CASE( test_binary_file_kmod_full_chunks )
{
    cout << "Check binary input: /bin/kmod fed in 2048-byte chunks (fallback: synthetic ELF blob)." << endl;

    ifstream binFile("/bin/kmod", ios::binary);

    unique_ptr<HTTPParser> parser = make_unique<HTTPParser>(4096);

    if (binFile.is_open()) {
        const size_t chunkSize = 2048;
        vector<char> chunk(chunkSize);

        // Feed up to 4 chunks (8 KB) of binary data
        for (int i = 0; i < 4 && binFile; ++i) {
            binFile.read(chunk.data(), chunkSize);
            streamsize bytesRead = binFile.gcount();
            if (bytesRead > 0) {
                BOOST_CHECK_NO_THROW(
                    parser->appendBuffer(chunk.data(), static_cast<uint16_t>(bytesRead))
                );
            }
        }
        binFile.close();
    } else {
        // /bin/kmod not available – use the synthetic ELF-like blob repeated
        // to fill the parser buffer with realistic binary content.
        BOOST_TEST_MESSAGE("/bin/kmod not found; using synthetic binary fallback");
        for (int i = 0; i < 64; ++i) {
            BOOST_CHECK_NO_THROW(
                parser->appendBuffer(
                    reinterpret_cast<const char*>(SYNTHETIC_BINARY_BLOB),
                    sizeof(SYNTHETIC_BINARY_BLOB)
                )
            );
        }
    }

    RequestsMap_t Requests;
    BOOST_CHECK_NO_THROW(Requests = parser->getRequests());

    cout << "Requests parsed from binary file chunks: " << Requests.size() << endl;
    // An ELF binary cannot form valid HTTP/1.1 GET or POST requests
    BOOST_TEST(Requests.size() == 0);
}

BOOST_AUTO_TEST_CASE( test_binary_file_kmod_byte_by_byte )
{
    cout << "Check binary input: /bin/kmod fed 1 byte at a time (first 512 bytes; fallback: synthetic ELF blob)." << endl;

    vector<char> data;
    ifstream binFile("/bin/kmod", ios::binary);

    if (binFile.is_open()) {
        data.resize(512);
        binFile.read(data.data(), data.size());
        streamsize bytesRead = binFile.gcount();
        data.resize(static_cast<size_t>(bytesRead));
        binFile.close();
    } else {
        // /bin/kmod not available – use the synthetic ELF-like blob
        BOOST_TEST_MESSAGE("/bin/kmod not found; using synthetic binary fallback");
        data.assign(
            reinterpret_cast<const char*>(SYNTHETIC_BINARY_BLOB),
            reinterpret_cast<const char*>(SYNTHETIC_BINARY_BLOB) + sizeof(SYNTHETIC_BINARY_BLOB)
        );
    }

    unique_ptr<HTTPParser> parser = make_unique<HTTPParser>(4096);

    for (size_t i = 0; i < data.size(); ++i) {
        BOOST_CHECK_NO_THROW(parser->appendBuffer(&data[i], 1));
    }

    RequestsMap_t Requests;
    BOOST_CHECK_NO_THROW(Requests = parser->getRequests());

    cout << "Requests parsed from binary file (byte-by-byte): " << Requests.size() << endl;
    BOOST_TEST(Requests.size() == 0);
}


// =========================================================================
// Category 5: Requests designed to stress-test / potentially crash the parser
// =========================================================================

BOOST_AUTO_TEST_CASE( test_crash_request_line_method_only )
{
    cout << "Check crash-inducing: request line with only a method (no URL or version)." << endl;

    const string bad_request("GET\r\n\r\n");

    unique_ptr<HTTPParser> parser = make_unique<HTTPParser>(4096);
    BOOST_CHECK_NO_THROW(parser->appendBuffer(bad_request.c_str(), bad_request.length()));
    RequestsMap_t Requests = parser->getRequests();

    cout << "Requests parsed: " << Requests.size() << endl;
    BOOST_TEST(Requests.size() == 0);
}

BOOST_AUTO_TEST_CASE( test_crash_request_line_missing_version )
{
    cout << "Check crash-inducing: request line missing HTTP version." << endl;

    const string bad_request("GET /test\r\nHost: example.com\r\n\r\n");

    unique_ptr<HTTPParser> parser = make_unique<HTTPParser>(4096);
    BOOST_CHECK_NO_THROW(parser->appendBuffer(bad_request.c_str(), bad_request.length()));
    RequestsMap_t Requests = parser->getRequests();

    cout << "Requests parsed: " << Requests.size() << endl;
    BOOST_TEST(Requests.size() == 0);
}

BOOST_AUTO_TEST_CASE( test_crash_request_line_wrong_version )
{
    cout << "Check crash-inducing: request line with wrong HTTP version (HTTP/2.0)." << endl;

    const string bad_request("GET /test HTTP/2.0\r\nHost: example.com\r\n\r\n");

    unique_ptr<HTTPParser> parser = make_unique<HTTPParser>(4096);
    BOOST_CHECK_NO_THROW(parser->appendBuffer(bad_request.c_str(), bad_request.length()));
    RequestsMap_t Requests = parser->getRequests();

    cout << "Requests parsed: " << Requests.size() << endl;
    // Only HTTP/1.1 is supported; HTTP/2.0 must be rejected
    BOOST_TEST(Requests.size() == 0);
}

BOOST_AUTO_TEST_CASE( test_crash_buffer_overflow_attempt )
{
    cout << "Check crash-inducing: attempt to overflow the internal buffer." << endl;

    // A string larger than the 4096-byte parser buffer
    const string oversize(8192, 'A');

    unique_ptr<HTTPParser> parser = make_unique<HTTPParser>(4096);
    // First call fills the buffer to maximum
    BOOST_CHECK_NO_THROW(parser->appendBuffer(oversize.c_str(), 4096));
    // Second call must be silently discarded (not crash)
    BOOST_CHECK_NO_THROW(parser->appendBuffer(oversize.c_str(), 4096));
    RequestsMap_t Requests = parser->getRequests();

    cout << "Requests parsed: " << Requests.size() << endl;
    BOOST_TEST(Requests.size() == 0);
}

BOOST_AUTO_TEST_CASE( test_crash_extremely_long_url )
{
    cout << "Check crash-inducing: GET with extremely long URL." << endl;

    // URL of 2000 chars; combined with headers just fits within a 4096-byte buffer
    const string long_url(2000, 'x');
    const string request("GET /" + long_url + " HTTP/1.1\r\nHost: x.x\r\n\r\n");

    unique_ptr<HTTPParser> parser = make_unique<HTTPParser>(4096);
    const uint16_t toSend = (request.length() <= 4096)
        ? static_cast<uint16_t>(request.length())
        : 4096u;
    BOOST_CHECK_NO_THROW(parser->appendBuffer(request.c_str(), toSend));
    RequestsMap_t Requests = parser->getRequests();

    cout << "Requests parsed: " << Requests.size() << endl;
    // Parser must not crash; result depends on whether the full request fits in the buffer
    BOOST_TEST(Requests.size() <= 1);
}

BOOST_AUTO_TEST_CASE( test_crash_post_without_content_length )
{
    cout << "Check crash-inducing: POST without Content-Length header." << endl;

    const string bad_request(
        "POST /submit HTTP/1.1\r\n"
        "Host: example.com\r\n"
        "Content-Type: text/plain\r\n"
        "\r\n"
        "hello"
    );

    unique_ptr<HTTPParser> parser = make_unique<HTTPParser>(4096);
    BOOST_CHECK_NO_THROW(parser->appendBuffer(bad_request.c_str(), bad_request.length()));
    RequestsMap_t Requests = parser->getRequests();

    cout << "Requests parsed: " << Requests.size() << endl;
    BOOST_TEST(Requests.size() == 0);
}

BOOST_AUTO_TEST_CASE( test_crash_post_nonnumeric_content_length )
{
    cout << "Check crash-inducing: POST with non-numeric Content-Length." << endl;

    const string bad_request(
        "POST /submit HTTP/1.1\r\n"
        "Host: example.com\r\n"
        "Content-Length: abc\r\n"
        "\r\n"
        "hello"
    );

    unique_ptr<HTTPParser> parser = make_unique<HTTPParser>(4096);
    BOOST_CHECK_NO_THROW(parser->appendBuffer(bad_request.c_str(), bad_request.length()));
    RequestsMap_t Requests = parser->getRequests();

    cout << "Requests parsed: " << Requests.size() << endl;
    BOOST_TEST(Requests.size() == 0);
}

BOOST_AUTO_TEST_CASE( test_crash_post_oversized_content_length )
{
    cout << "Check crash-inducing: POST with Content-Length exceeding maximum allowed." << endl;

    // HTTP_POST_MAX_CONTENT_LENGTH is 4096; use 65535 to exceed it
    const string bad_request(
        "POST /submit HTTP/1.1\r\n"
        "Host: example.com\r\n"
        "Content-Length: 65535\r\n"
        "\r\n"
        "payload"
    );

    unique_ptr<HTTPParser> parser = make_unique<HTTPParser>(4096);
    BOOST_CHECK_NO_THROW(parser->appendBuffer(bad_request.c_str(), bad_request.length()));
    RequestsMap_t Requests = parser->getRequests();

    cout << "Requests parsed: " << Requests.size() << endl;
    BOOST_TEST(Requests.size() == 0);
}

BOOST_AUTO_TEST_CASE( test_crash_post_zero_content_length )
{
    cout << "Check edge case: POST with Content-Length: 0 (empty body)." << endl;

    const string request(
        "POST /submit HTTP/1.1\r\n"
        "Host: example.com\r\n"
        "Content-Length: 0\r\n"
        "\r\n"
    );

    unique_ptr<HTTPParser> parser = make_unique<HTTPParser>(4096);
    BOOST_CHECK_NO_THROW(parser->appendBuffer(request.c_str(), request.length()));
    RequestsMap_t Requests = parser->getRequests();

    cout << "Requests parsed: " << Requests.size() << endl;
    // A POST with Content-Length: 0 is valid; must produce 1 request with empty payload
    BOOST_TEST(Requests.size() == 1);
    BOOST_TEST(Requests.at(0).HTTPMethod == HTTP_METHOD_POST);
    BOOST_TEST(Requests.at(0).Payload == "");
}

BOOST_AUTO_TEST_CASE( test_crash_request_with_null_bytes )
{
    cout << "Check crash-inducing: valid-looking request with embedded null bytes." << endl;

    // Build a request that looks valid but contains a null byte in the header section
    string request("GET /test HTTP/1.1\r\nX-Header: value");
    request.push_back('\0');
    request += "\r\nHost: example.com\r\n\r\n";

    unique_ptr<HTTPParser> parser = make_unique<HTTPParser>(4096);
    BOOST_CHECK_NO_THROW(parser->appendBuffer(request.c_str(), request.length()));
    RequestsMap_t Requests = parser->getRequests();

    cout << "Requests parsed: " << Requests.size() << endl;
    // Must not crash; the request line itself is valid
    BOOST_TEST(Requests.size() == 1);
    BOOST_TEST(Requests.at(0).URL == "/test");
}

BOOST_AUTO_TEST_CASE( test_crash_mixed_valid_invalid_requests )
{
    cout << "Check crash-inducing: valid GET, then invalid DELETE, then valid POST in one buffer." << endl;

    const string mixed(
        "GET /valid HTTP/1.1\r\nHost: example.com\r\n\r\n"
        "DELETE /invalid HTTP/1.1\r\nHost: example.com\r\n\r\n"
        "POST /valid HTTP/1.1\r\nHost: example.com\r\nContent-Length: 2\r\n\r\n{}"
    );

    unique_ptr<HTTPParser> parser = make_unique<HTTPParser>(4096);
    BOOST_CHECK_NO_THROW(parser->appendBuffer(mixed.c_str(), mixed.length()));
    RequestsMap_t Requests = parser->getRequests();

    cout << "Requests parsed: " << Requests.size() << endl;
    // The GET is parsed first; DELETE causes the loop to break; POST is never reached
    BOOST_TEST(Requests.size() == 1);
    BOOST_TEST(Requests.at(0).HTTPMethod == HTTP_METHOD_GET);
    BOOST_TEST(Requests.at(0).URL == "/valid");
}

BOOST_AUTO_TEST_CASE( test_crash_request_leading_space_in_method )
{
    cout << "Check crash-inducing: request line with a leading space before the method." << endl;

    const string bad_request(" GET /test HTTP/1.1\r\nHost: example.com\r\n\r\n");

    unique_ptr<HTTPParser> parser = make_unique<HTTPParser>(4096);
    BOOST_CHECK_NO_THROW(parser->appendBuffer(bad_request.c_str(), bad_request.length()));
    RequestsMap_t Requests = parser->getRequests();

    cout << "Requests parsed: " << Requests.size() << endl;
    // A leading space produces an extra token; the size check (!=3) rejects the request
    BOOST_TEST(Requests.size() == 0);
}

BOOST_AUTO_TEST_CASE( test_crash_repeated_large_garbage_chunks )
{
    cout << "Check crash-inducing: repeated large garbage inputs filling and overflowing buffer." << endl;

    // Vary the byte pattern to avoid accidentally forming valid HTTP
    const string chunk1(1024, '\x01');
    const string chunk2(1024, '\xFF');
    const string chunk3(1024, '\xAB');
    const string chunk4(1024, '\x7F');
    const string chunk5(1024, '\x42');  // extra chunk that must be discarded

    unique_ptr<HTTPParser> parser = make_unique<HTTPParser>(4096);
    BOOST_CHECK_NO_THROW(parser->appendBuffer(chunk1.c_str(), chunk1.length()));
    BOOST_CHECK_NO_THROW(parser->appendBuffer(chunk2.c_str(), chunk2.length()));
    BOOST_CHECK_NO_THROW(parser->appendBuffer(chunk3.c_str(), chunk3.length()));
    BOOST_CHECK_NO_THROW(parser->appendBuffer(chunk4.c_str(), chunk4.length()));
    // This call exceeds the buffer limit and must be silently discarded
    BOOST_CHECK_NO_THROW(parser->appendBuffer(chunk5.c_str(), chunk5.length()));
    RequestsMap_t Requests = parser->getRequests();

    cout << "Requests parsed: " << Requests.size() << endl;
    BOOST_TEST(Requests.size() == 0);
}
