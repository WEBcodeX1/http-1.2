#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>
#include <memory>

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

    RequestsVector_t Requests;

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

    RequestsVector_t Requests;

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

    RequestsVector_t Requests;

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

    RequestsVector_t Requests;

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

    RequestsVector_t Requests;

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

    RequestsVector_t Requests;

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

    RequestsVector_t Requests;

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

    RequestsVector_t Requests;

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

    RequestsVector_t Requests;

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
    RequestsVector_t Requests;

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

    RequestsVector_t Requests;

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

    RequestsVector_t Requests;

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

    RequestsVector_t Requests;

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
