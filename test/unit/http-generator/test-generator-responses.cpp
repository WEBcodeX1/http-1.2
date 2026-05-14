#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>
#include <memory>
#include <iostream>
#include <string>
#include <regex>

#include "../../../lib/http/httpgenerator.hpp"

using namespace std;


// ── helper ──────────────────────────────────────────────────────────────────

static bool contains(const string& haystack, const string& needle)
{
    return haystack.find(needle) != string::npos;
}

static bool startsWith(const string& s, const string& prefix)
{
    return s.rfind(prefix, 0) == 0;
}


// ── status line ──────────────────────────────────────────────────────────────

BOOST_AUTO_TEST_CASE( test_default_status_line )
{
    cout << "Check default status line (200 OK)." << endl;

    unique_ptr<HTTPGenerator> gen = make_unique<HTTPGenerator>();
    string response = gen->generate();

    cout << "Response: " << response << endl;

    BOOST_TEST(startsWith(response, "HTTP/1.1 200 OK\r\n"));
}

BOOST_AUTO_TEST_CASE( test_custom_status_code_and_text )
{
    cout << "Check custom status code and text (404 Not Found)." << endl;

    unique_ptr<HTTPGenerator> gen = make_unique<HTTPGenerator>();
    gen->setStatus(404, "Not Found");
    string response = gen->generate();

    cout << "Response: " << response << endl;

    BOOST_TEST(startsWith(response, "HTTP/1.1 404 Not Found\r\n"));
}

BOOST_AUTO_TEST_CASE( test_500_internal_server_error_status )
{
    cout << "Check 500 Internal Server Error status line." << endl;

    unique_ptr<HTTPGenerator> gen = make_unique<HTTPGenerator>();
    gen->setStatus(500, "Internal Server Error");
    string response = gen->generate();

    cout << "Response: " << response << endl;

    BOOST_TEST(startsWith(response, "HTTP/1.1 500 Internal Server Error\r\n"));
}


// ── body and content-length ──────────────────────────────────────────────────

BOOST_AUTO_TEST_CASE( test_empty_body_content_length_zero )
{
    cout << "Check Content-Length: 0 for empty body." << endl;

    unique_ptr<HTTPGenerator> gen = make_unique<HTTPGenerator>();
    string response = gen->generate();

    cout << "Response: " << response << endl;

    BOOST_TEST(contains(response, "Content-Length: 0\r\n"));
}

BOOST_AUTO_TEST_CASE( test_body_appears_in_response )
{
    cout << "Check that body appears after header section." << endl;

    const string body = "{\"status\":\"ok\"}";
    unique_ptr<HTTPGenerator> gen = make_unique<HTTPGenerator>();
    gen->setBody(body);
    string response = gen->generate();

    cout << "Response: " << response << endl;

    // Body must appear after the blank line
    auto sep = response.find("\r\n\r\n");
    BOOST_TEST(sep != string::npos);
    string actual_body = response.substr(sep + 4);
    BOOST_TEST(actual_body == body);
}

BOOST_AUTO_TEST_CASE( test_content_length_matches_body )
{
    cout << "Check Content-Length header matches actual body size." << endl;

    const string body = "Hello, World!";
    unique_ptr<HTTPGenerator> gen = make_unique<HTTPGenerator>();
    gen->setBody(body);
    string response = gen->generate();

    cout << "Response: " << response << endl;

    string expected_cl = "Content-Length: " + to_string(body.size()) + "\r\n";
    BOOST_TEST(contains(response, expected_cl));
}

BOOST_AUTO_TEST_CASE( test_binary_body_content_length )
{
    cout << "Check Content-Length for body containing null bytes." << endl;

    const string body("ab\0cd", 5);
    unique_ptr<HTTPGenerator> gen = make_unique<HTTPGenerator>();
    gen->setBody(body);
    string response = gen->generate();

    string expected_cl = "Content-Length: 5\r\n";
    BOOST_TEST(contains(response, expected_cl));
}


// ── headers ──────────────────────────────────────────────────────────────────

BOOST_AUTO_TEST_CASE( test_add_single_header )
{
    cout << "Check single custom header appears in response." << endl;

    unique_ptr<HTTPGenerator> gen = make_unique<HTTPGenerator>();
    gen->addHeader("Content-Type", "text/html");
    string response = gen->generate();

    cout << "Response: " << response << endl;

    BOOST_TEST(contains(response, "Content-Type: text/html\r\n"));
}

BOOST_AUTO_TEST_CASE( test_add_multiple_headers )
{
    cout << "Check multiple custom headers appear in response." << endl;

    unique_ptr<HTTPGenerator> gen = make_unique<HTTPGenerator>();
    gen->addHeader("Content-Type", "application/json");
    gen->addHeader("X-Custom-Header", "custom-value");
    gen->addHeader("Cache-Control", "no-cache");
    string response = gen->generate();

    cout << "Response: " << response << endl;

    BOOST_TEST(contains(response, "Content-Type: application/json\r\n"));
    BOOST_TEST(contains(response, "X-Custom-Header: custom-value\r\n"));
    BOOST_TEST(contains(response, "Cache-Control: no-cache\r\n"));
}

BOOST_AUTO_TEST_CASE( test_headers_appear_before_body_separator )
{
    cout << "Check headers appear before the blank-line separator." << endl;

    unique_ptr<HTTPGenerator> gen = make_unique<HTTPGenerator>();
    gen->addHeader("X-Test", "value");
    gen->setBody("body");
    string response = gen->generate();

    auto header_pos = response.find("X-Test: value\r\n");
    auto sep_pos    = response.find("\r\n\r\n");

    BOOST_TEST(header_pos != string::npos);
    BOOST_TEST(sep_pos    != string::npos);
    BOOST_TEST(header_pos < sep_pos);
}


// ── CRLF injection guard ──────────────────────────────────────────────────────

BOOST_AUTO_TEST_CASE( test_crlf_in_header_value_is_rejected )
{
    cout << "Check header with \\r\\n in value is silently skipped." << endl;

    unique_ptr<HTTPGenerator> gen = make_unique<HTTPGenerator>();
    gen->addHeader("X-Safe", "good-value");
    gen->addHeader("X-Evil", "bad\r\nvalue");
    string response = gen->generate();

    cout << "Response: " << response << endl;

    BOOST_TEST( contains(response, "X-Safe: good-value\r\n"));
    BOOST_TEST(!contains(response, "X-Evil"));
}

BOOST_AUTO_TEST_CASE( test_cr_only_in_header_value_is_rejected )
{
    cout << "Check header with \\r only in value is silently skipped." << endl;

    unique_ptr<HTTPGenerator> gen = make_unique<HTTPGenerator>();
    gen->addHeader("X-Evil-CR", "bad\rvalue");
    string response = gen->generate();

    BOOST_TEST(!contains(response, "X-Evil-CR"));
}

BOOST_AUTO_TEST_CASE( test_lf_only_in_header_value_is_rejected )
{
    cout << "Check header with \\n only in value is silently skipped." << endl;

    unique_ptr<HTTPGenerator> gen = make_unique<HTTPGenerator>();
    gen->addHeader("X-Evil-LF", "bad\nvalue");
    string response = gen->generate();

    BOOST_TEST(!contains(response, "X-Evil-LF"));
}

BOOST_AUTO_TEST_CASE( test_crlf_in_header_name_is_rejected )
{
    cout << "Check header with \\r\\n in name is silently skipped." << endl;

    unique_ptr<HTTPGenerator> gen = make_unique<HTTPGenerator>();
    gen->addHeader("X-Evil\r\nInjected", "value");
    string response = gen->generate();

    BOOST_TEST(!contains(response, "Injected"));
}


// ── Date header ───────────────────────────────────────────────────────────────

BOOST_AUTO_TEST_CASE( test_add_date_header_present )
{
    cout << "Check addDateHeader() inserts a Date header." << endl;

    unique_ptr<HTTPGenerator> gen = make_unique<HTTPGenerator>();
    gen->addDateHeader();
    string response = gen->generate();

    cout << "Response: " << response << endl;

    BOOST_TEST(contains(response, "Date: "));
}

BOOST_AUTO_TEST_CASE( test_add_date_header_ends_with_gmt )
{
    cout << "Check Date header value ends with GMT." << endl;

    unique_ptr<HTTPGenerator> gen = make_unique<HTTPGenerator>();
    gen->addDateHeader();
    string response = gen->generate();

    // Find "Date: " and extract value up to the next \r\n
    auto pos = response.find("Date: ");
    BOOST_TEST(pos != string::npos);

    auto end = response.find("\r\n", pos);
    BOOST_TEST(end != string::npos);

    string date_value = response.substr(pos + 6, end - pos - 6);
    cout << "Date value: " << date_value << endl;

    BOOST_TEST(date_value.size() >= 3);
    BOOST_TEST(date_value.substr(date_value.size() - 3) == "GMT");
}

BOOST_AUTO_TEST_CASE( test_add_date_header_format )
{
    cout << "Check Date header matches RFC 7231 format." << endl;

    unique_ptr<HTTPGenerator> gen = make_unique<HTTPGenerator>();
    gen->addDateHeader();
    string response = gen->generate();

    auto pos = response.find("Date: ");
    BOOST_TEST(pos != string::npos);

    auto end = response.find("\r\n", pos);
    string date_value = response.substr(pos + 6, end - pos - 6);
    cout << "Date value: " << date_value << endl;

    // e.g. "Thu, 14 May 2026 09:03:24 GMT"
    regex date_re(R"([A-Za-z]{3}, \d{2} [A-Za-z]{3} \d{4} \d{2}:\d{2}:\d{2} GMT)");
    BOOST_TEST(regex_match(date_value, date_re));
}


// ── full response structure ───────────────────────────────────────────────────

BOOST_AUTO_TEST_CASE( test_full_response_structure )
{
    cout << "Check complete HTTP/1.1 response structure." << endl;

    const string body = "Hello";
    unique_ptr<HTTPGenerator> gen = make_unique<HTTPGenerator>();
    gen->setStatus(200, "OK");
    gen->addHeader("Content-Type", "text/plain");
    gen->setBody(body);
    string response = gen->generate();

    cout << "Response: " << response << endl;

    // Must start with status line
    BOOST_TEST(startsWith(response, "HTTP/1.1 200 OK\r\n"));

    // Must have a custom header
    BOOST_TEST(contains(response, "Content-Type: text/plain\r\n"));

    // Must have Content-Length
    BOOST_TEST(contains(response, "Content-Length: 5\r\n"));

    // Must have blank-line separator
    BOOST_TEST(contains(response, "\r\n\r\n"));

    // Must end with body
    BOOST_TEST(response.substr(response.size() - body.size()) == body);
}
